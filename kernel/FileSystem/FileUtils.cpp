
#include "FileUtils.h"
#include "types.h"
#include "VFS.h"
#include "errs.h"
#include "logging.h"

shared_ptr<Vector<u8>> FileUtils::read_all(CharFile& f, size_t& size) {
    size = f.size();
    // TODO: use a BigBuffer here
    shared_ptr<Vector<u8>> buff(new Vector<u8>(size));
    int res = f.read(size, buff->data());
    kprintf("res: %d, size: %d\n", res, size);
    if(res != (int)size) {
        return nullptr;
    }
    return buff;
}

int FileUtils::file_size(const String& path) {
    File* f = VFS::the().open(Path(path));
    if(f == nullptr) {
        return E_NOTFOUND;
    }
    // TODO: dynamic_cast
    auto* char_f = static_cast<CharFile*>(f);
    return char_f->size();
    
}