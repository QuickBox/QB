struct lock;

namespace xmlrpc_c {

class Lock {
public:
    class Holder {
    public:
        Holder(Lock * const lockP);

        ~Holder();

    private:
        Lock * const lockP;
    };

    Lock();

    ~Lock();

    void
    acquire();

    void
    release();

private:

    lock * const c_lockP;
};

}  // namespace
