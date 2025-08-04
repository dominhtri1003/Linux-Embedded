#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/reboot.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/moduleparam.h>
#include <linux/pm_runtime.h>
#include <linux/watchdog.h>

static bool nowayout = WATCHDOG_NOWAYOUT;
// Cho phep bien nowayout co the thay doi dc
module_param(nowayout, bool, 0000);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
		__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static unsigned int timer_margin;
module_param(timer_margin, uint, 0000);
MODULE_PARM_DESC(timer_margin, "Initial watchdog timeout (in seconds)");
                    
#define PTV			            2
#define GET_WLDR_VAL(secs)      (0xffffffff - ((secs) * (32768/(1<<PTV))) + 1)
#define BBB_MAX_TIMEOUT		    (24 * 60 * 60)	/* 1 day */
#define BBB_MIN_TIMEOUT		    1		/* 1 sec */
#define BBB_TIMEOUT_DEFAULT     60      /* 60 secs */

#define WDT_WIDR		(0x00)
#define WDT_WDSC		(0x10)
#define WDT_WDST		(0x14)
#define WDT_WISR		(0x18)
#define WDT_WIER		(0x1C)
#define WDT_WCLR		(0x24)
#define WDT_WCRR		(0x28)
#define WDT_WLDR		(0x2C)
#define WDT_WTGR		(0x30)
#define WDT_WWPS		(0x34)
#define WDT_WDLY		(0x44)
#define WDT_WSPR		(0x48)

struct bbb_wdt_dev
{
    struct watchdog_device wdog;
    void __iomem *base;
    struct device *dev;
    bool    is_active_wdog;
    int     wdt_trigg_val;
    struct mutex lock; 
};

// Ghi 2 gia tri 0xBBBB va` 0x4444 vao` WSPR
// Doc gia tri 
static void bbb_wdt_enable(struct bbb_wdt_dev *wdev)
{
    // Khai bao con tro base kieu void tro toi vung` nho I/O(Input/Output Memory)
    // Lay dia chi base addr cua thanh ghi watchdog tu` struct wdev
    // Gan dia chi do vao` base
    void __iomem *base = wdev->base;

    iowrite32(0xBBBB, (base + WDT_WSPR));
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    // WWPS: Watchdog Write Protect Status Register
    // Kiem tra xem bit 4 cua WWPS co bang 1 khong,
    while(readl_relaxed(base + WDT_WWPS) & (1 << 4))
        cpu_relax();
    pr_emerg("TriDo %s, %d", __func__, __LINE__);
    
    iowrite32(0x4444, (base + WDT_WSPR));
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    while(readl_relaxed(base + WDT_WWPS) & (1 << 4))
        cpu_relax();
    pr_emerg("TriDo %s, %d", __func__, __LINE__);   
}

static void bbb_wdt_disable(struct bbb_wdt_dev *wdev)
{
    void __iomem *base = wdev->base;

    iowrite32(0xAAAA, (base + WDT_WSPR));
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    while(readl_relaxed(base + WDT_WWPS) & (1 << 4))
        cpu_relax();
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    iowrite32(0x5555, (base + WDT_WSPR));
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    while(readl_relaxed(base + WDT_WWPS) & (1 << 4))
        cpu_relax();
}

// Reset dong` ho` watchdog -> he thong khong bi khoi dong lai 1 cach khong mong muon
static void bbb_wdt_reload(struct bbb_wdt_dev *wdev)
{
    void __iomem *base = wdev->base;

    // Phep dao? bit thay doi gia tri bien wdt_trigg_val trong struct wdev tao ra 1 gia tri moi
    // Khac voi gia tri truoc do
    // Giup bao ve khoi viec reset he thong. dam? bao? mot gia tri khong doi
    // Khong the kich hoat lai watchdog
    wdev->wdt_trigg_val = ~wdev->wdt_trigg_val;

    /* Wait for posted write to complete */
    while(readl_relaxed(base + WDT_WWPS) & (1 << 3))
        cpu_relax();
    
    iowrite32(wdev->wdt_trigg_val, (base + WDT_WTGR));
    /* Wait for posted write to complete */
    while(readl_relaxed(base + WDT_WWPS) & (1 <<3))
        cpu_relax();
}

// Dat gia tri timeout vao` thanh ghi phan` cung cho watchdog
static void bbb_wdt_set_timer(struct bbb_wdt_dev *wdev, unsigned int timeout))
{
    void __iomem *base = wdev->base;
    // Chuyen doi gia tri timeout thanh gia tri phu` hop de luu vao` thanh ghi WDT_WLDR
    // Do watchdog khong dem bang giay ma` dem bang xung clock
    int load_val = GET_WLDR_VAL(timeout);

    while(readl_relaxed(base + WDT_WWPS) & (1 << 2))
        cpu_relax();
    // Ghi gia tri load_val da~ dc chuyen doi vao` thanh ghi WDT_WLDR
    iowrite32(load_val, (base + WDT_WLDR));

    while(readl_relaxed(base + WDT_WWPS) & (1 << 2))
        cpu_relax();
}

// Ham cau hinh` va` khoi tao watchdog
static void bbb_wdt_setup(struct bbb_wdt_dev *wdev)
{
    void __iomem *base = wdev->base;

    mutex_lock(&wdev->lock);

    // Disable watchdog truoc khi cau hinh` lai
    bbb_wdt_disable(wdev);

    /* Set prescaler */
    while(readl_relaxed(base + WDT_WWPS) & 0x01)
        cpu_relax();
    
    /* PTV = 2 */
    iowrite32((1 << 5) | (1 << 2), (base + WDT_WCLR));
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    bbb_wdt_set_timer(wdev, wdev->wdog.timeout);
    bbb_wdt_reload(wdev);
    bbb_wdt_enable(wdev);

    wdev->is_active_wdog = true;

    mutex_unlock(&wdev->lock);
}

/*  
    Ham` callback chuan cua framework linux watchdog
    Quan ly toan` bo qua trinh thay doi gia tri timeout cua watchdog
    Duoc kernel goi khi nguoi` dung` hoac he thong muong thay doi gia tri timeout cua watchdog 
*/
static int bbb_wdt_set_timeout(struct watchdog_device *wdog, unsigned int timeout)
{
    // watchdog_get_drvdata(wdog): ham` cua linux watchdog framlinux chua thong tin chung ma` kernel qua ly watchdog
    // wdog: con tro toi struct watchdog_device cua framework linux watchdog
    // Day la` struct chung kernel dung` de quan ly cac thiet bi watchdog
    // Khong chua thong tin cua thiet bi watchdog cu the.
    // ------------------------------------------------- //
    // Lay con tro toi struct bbb_wdt_dev tu` struct watchdog_device (con tro chung cua kernel)
    struct bbb_wdt_dev *wdev = watchdog_get_drvdata(wdog);

    mutex_lock(&wdev->lock);

    bbb_wdt_disable(wdev);
    bbb_wdt_set_timer(wdev, timeout);
    bbb_wdt_enable(wdev);
    bbb_wdt_reload(wdev);
    wdog->timeout = timeout;

    mutex_unlock(&wdev->lock);

    pr_emerg("TriDo %s, %d, timeout = %d", __func__, __LINE__, timeout);
    return 0;
}

static int bbb_wdt_ping(struct watchdog_device *wdog)
{
    struct bbb_wdt_dev *wdev = watchdog_get_drvdata(wdog);
    mutex_lock(&wdev->lock);
    bbb_wdt_reload(wdev);
    mutex_unlock(&wdev->lock);
    pr_emerg("TriDo %s, %d", __func__, __LINE__);
    return 0;
}

static int bbb_wdt_start(struct watchdog_device *wdog)
{
    struct bbb_wdt_dev *wdev = watchdog_get_drvdata(wdog);

    pm_runtime_get_sync(wdev->dev);

    if(wdev->is_active_wdog == true)
        bbb_wdt_set_timeout(wdog, wdog->timeout);
    else
        bbb_wdt_setup(wdev);

    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    return bbb_wdt_ping(wdog);
}

// Vo hieu hoa watwatch, ngan 1 lan khoi dong lai khong mong muon
static int bbb_wdt_stop(struct watchdog_device *wdog)
{
    struct bbb_wdt_dev *wdev = watchdog_get_drvdata(wdog);

    mutex_lock(&wdev->lock);
    // Tranh khoi dong lai he thong truoc khi disable watchdog
    bbb_wdt_ping(wdog);
    wdev->is_active_wdog = false;
    bbb_wdt_disable(wdev);
    pr_emerg("TriDo %s, %d", __func__, __LINE__);
    mutex_unlock(&wdev->lock);
    return 0;
}

static const struct watchdog_info omap_wdt_info = {
    .options = WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE | WDIOF_KEEPALIVEPING,
    .identity = "BBB Watchdog",
};

static const struct watchdog_ops bbb_wdt_ops = {
    .owner          = THIS_MODULE,
    .start          = bbb_wdt_start,
    .stop           = bbb_wdt_stop,
    .ping           = bbb_wdt_ping,
    .set_timeout    = bbb_wdt_set_timeout,
};

static int bbb_wdt_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct resource *res;
    struct bbb_wdt_dev = NULL;

    // devm_kzalloc ham` cap phat vung` nho trong nhan linux
    // Bo nho dc tu dong giai phong khi thiet bi bi remove/unbind
    // GFP_KERNEL: Khoi tao trong ngu~ canh kernel
    // Cho phep cho` neu bo nho ko san sang`
    wdev = devm_kzalloc(&pdev->dev, sizeof(*wdev), GFP_KERNEL);
    if (WDEV == NULL)
        return -ENOMEM;
    pr_emerg("TriDo %s, %d", __func__, __LINE__);

    wdev->is_active_wdt = false;
    wdev->wdt_trigg_val = 0x1003;
    wdev->dev = &pdev->dev;
    mutex_init(&wdev->lock);

    res = platform_get_resource(pdev, IORESOURCE_NEM, 0):
    if (res == NULL)
    {
        pr_emerg("TriDo %s, %d, Failed to get resource", __func__, __LINE__);
        goto resource_not_exit;
    }

    wdev->base = devm_ioremap_resource(&pdev->dev, res);
    if(IS_ERR(wdev->base))
    {
        pr_emerg("TriDo %s, %d, Failed to ioremap resource", __func__, __LINE__);
        return PTR_ERR(wdev->base);
    }

    wdev->wdog.parent = &pdev->dev;
    wdev->wdog.ops = &bbb_wdt_ops;
    wdev->wdog.info = &omap_wdt_info;
    wdev->wdog.min_timeout = BBB_MIN_TIMEOUT;
    wdev->wdog.max_timeout = BBB_MAX_TIMEOUT;
    wdev->wdog.status = WATCHDOG_NOAWAYOUT_INIT_STATUS;

    if(watchdog_init_timeout(&wdev->wdog, timer_margin, &pdev->dev) < 0)
    {
        wdev->wdog.timeout = BBB_TIMEOUT_DEFAULT;
        pr_emerg("TriDo %s, %d, Failed to ioremap resource", __func__, __LINE__);
    }

    watchdog_set_nowayout(&wdev->wdog, nowayout);

    pm_runtime_enable(wdev->dev);
    pm_runtime_get_sync(wdev->dev);

    /* Step 1 */
    platform_set_drvdata(pdev, wdev);
    watchdog_set_drvdata(&wdev->wdog, wdev);

    /* Step 2 */
    ret = watchdog_register_device(&wdev->wdog);
    if(ret < 0)
    {
        pr_emerg("TriDo %s, %d, Failed to ioremap resource", __func__, __LINE__);
        goto register_watchdog_failed;
    }
    pr_info("BBB Watchdog timer Rev 0x%02x: initial timeout %d sec\n",
            readl_relaxed(wdev->base + WDT_WIDR) & 0xFF,
            wdev->wdog.timeout);
    
    bbb_wdt_disable(wdev);
    pm_runtime_put(wdev->dev);

    return 0;

register_watchdog_failed:
resource_not_exit:
    devm_kfree(&pdev->dev, wdev);
    return -1;
}

static int bbb_wdt_remove(struct platform_device *pdev)
{
    struct watchdog_device *wdog = platform_get_drvdata(pdev);
    struct sac58r_wdt_device *wdev = watchdog_get_drvdata(wdog);

    bbb_wdt_stop(wdog);
    watchdog_unregister_device(wdog);
    devm_kfree(&pdev->dev, wdev);

    return 0;
}

static const struct of_device_id bbb_wdt_of_match[] = {
    { . compatible = "trido,omap3-wdt", },
    {},
};
MODULE_DEVICE_TABLE(of, bbb_wdt_of_match);

static struct platform_driver bbb_wdt_driver = {
    .probe = bbb_wdt_probe,
    .remove = bbb_wdt_remove,
    .driver = {
        .name = "bbb_wdt",
        .of_match_table = bbb_wdt_of_match,
    },
};
module_platform_driver(bbb_wdt_driver);

MODULE_AUTHOR("Tri Do");
MODULE_LICENSE("GPL");