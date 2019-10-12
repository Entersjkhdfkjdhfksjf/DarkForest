#include "PageDirectory.h"
#include "MM/MM_types.h"
#include "Kassert.h"
#include "PageTable.h"

PDE PageDirectory::get_pde(VirtualAddress addr) {
    u32 pd_index = get_index(addr);
    kprintf("PD index for: 0x%x is: %d\n", addr, pd_index);
    ASSERT(pd_index < NUM_PAGE_DIRECTORY_ENTRIES, "index < num PD entries");
    return PDE(entries()[pd_index]);
}


u32 PageDirectory::get_cr3() {
    u32 val;
    asm("movl %%cr3, %%eax" : "=a"(val));
    return val;
}

u32* PageDirectory::entries() {
    return (u32*)((u32)m_addr);
}

u32 PageDirectory::get_index(VirtualAddress addr) {
    return (((u32)addr) >> 22) & 0x3ff;
}

// void PageDirectory::commit_pte(PTE pte, VirtualAddress addr) {
//     auto pde = get_pde(addr);
//     ASSERT(pde.is_present(), "commit_pte: PDE for addr is not present");
//     auto page_table = PageTable(pde.get_page_table());
//     page_table.entries()[PageTable::get_index(addr)] = pte.to_raw();
// }