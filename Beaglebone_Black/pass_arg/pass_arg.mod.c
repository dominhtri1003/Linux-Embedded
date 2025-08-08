#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x6ce69737, "param_set_int" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xdf394d23, "param_get_int" },
	{ 0xc368a5e1, "param_array_ops" },
	{ 0xc368a5e1, "param_ops_int" },
	{ 0xc368a5e1, "param_ops_charp" },
	{ 0xd272d446, "__fentry__" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x6ce69737,
	0xe8213e80,
	0xd272d446,
	0x90a48d82,
	0xdf394d23,
	0xc368a5e1,
	0xc368a5e1,
	0xc368a5e1,
	0xd272d446,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"param_set_int\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"__ubsan_handle_out_of_bounds\0"
	"param_get_int\0"
	"param_array_ops\0"
	"param_ops_int\0"
	"param_ops_charp\0"
	"__fentry__\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "009A6D3AD8A261662AA7534");
