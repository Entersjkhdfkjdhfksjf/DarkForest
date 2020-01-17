#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "asserts.h"
#include "string.h"
#include "drivers/DebugPort.h"
#include "cpu.h"
#include "logging.h"
#include "flags.h"
#include "MM/MemoryManager.h"
#include "MM/MM_types.h"
#include "kmalloc.h"
#include "task.h"
#include "drivers/PIC.h"
#include "drivers/PIT.h"
#include "Scheduler.h"
#include "drivers/PS2Keyboard.h"
#include "sleep.h"
#include "syscall.h"
#include "Loader/loader.h"
#include "FileSystem/VFS.h"
#include "FileSystem/DevFS.h"
#include "HAL/KeyboardDevice.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/FileUtils.h"
#include "kernel_symbols.h"
#include "shared_ptr.h"
#include "HAL/VgaTTY.h"
#include "drivers/ATADisk.h"
#include "FileSystem/Fat32FS.h"

#ifdef TESTS
#include "tests/tests.h"
#endif

 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__) || !defined(__i386__)
#error "You must compile with a cross compiler for i386"
#endif
 

void hello_world_userspace() {
	load_and_jump_userspace("/root/bin/hello.app");
}

void terminal_userspace() {
	load_and_jump_userspace("/root/bin/terminal.app");
}

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

void init_VFS() {
	VFS::the().mount(&DevFS::the());
	VFS::the().mount(&Fat32FS::the());
}

void init_kernel_symbols() {
	#ifdef KERNEL_SYMBOLS_ENABLED
	KernelSymbols::initialize();
	#endif
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("*******\nkernel_main\n*******\n\n");
	VgaTTY::the().write("DarKForest booting...\n");
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC);
	kprintf("I smell %x\n", 0xdeadbeef);
	kprintf("Multiboot modules: mods_count: %d\n, mods_addr: 0x%x\n", mbt->mods_count, mbt->mods_addr);
	VgaTTY::the().write("Initializing PIC devices...\n");
	PIC::initialize();
	init_descriptor_tables();
	PIT::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_ETERNAL);
	MemoryManager::initialize(mbt);
	KernelHeapAllocator::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_NORMAL);

	PS2Keyboard::initialize();
	ATADisk::initialize();

	VgaTTY::the().write("Initializing File Systems...\n");
	DevFS::initiailize();
	Fat32FS::initialize();
	// cpu_hang();


	init_VFS();

	// Vector<DirectoryEntry> res;
	// ASSERT(VFS::the().list_directory(Path("/root/a"), res));
	// for(auto& entry : res)
	// {
	// 	kprintf("%s\n", entry.path.to_string().c_str());
	// }
	// kprintf("# entries: %d\n", res.size());
	// cpu_hang();

	VgaTTY::the().write("Loading kernel symbols...\n");
	init_kernel_symbols();

#ifdef TESTS
	run_tests();
	return;
#endif

	init_syscalls();
	VgaTTY::the().write("Initizliaing the Scheduler...\n");
	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();

	// can't have them run concurrently becuase of FAT races
	// TODO: add locks to FAT
	// Scheduler::the().add_process(Process::create(hello_world_userspace, "HelloWorldUser"));
	Scheduler::the().add_process(Process::create(terminal_userspace, "TerminalUser"));

	// VFS::the().open("/inird/helllo.txt");
	// XASSERT(false);

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");

	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
