#include <iostream>
using namespace std;

class SingleCore
{
    public:
        virtual void Show() = 0;
};

class SingleCoreA : public SingleCore
{
    public:
        void Show()
        {
            cout << "SingleCoreA " << endl;
        }
};

class SingleCoreB : public SingleCore
{
    public:
        void Show()
        {
            cout << "SingleCoreB " << endl;
        }
};

class MultiCore
{
    public:
        virtual void Show() = 0;
};

class MultiCoreA : public MultiCore
{
    public:
        void Show()
        {
            cout << "MuitiCoreA " << endl;
        };
};

class MultiCoreB : public MultiCore
{
    public:
        void Show()
        {
            cout << "MuitiCoreB " << endl;
        };
};

class CoreFactory
{
    public:
        virtual SingleCore* createSingleCore() = 0;
        virtual MultiCore* createMultiCore() = 0;
};

class CoreFactoryA : public CoreFactory
{
    public:
        SingleCore* createSingleCore()
        {
            cout << "create SingleCoreA " << endl;
            return new SingleCoreA();
        };
        MultiCore* createMultiCore()
        {
            cout << "create MultiCoreA " << endl;
            return new MultiCoreA();
        };
};

class CoreFactoryB : public CoreFactory
{
    public:
        SingleCore* createSingleCore()
        {
            cout << "create SingleCoreB " << endl;
            return new SingleCoreB();
        };
        MultiCore* createMultiCore()
        {
            cout << "create MultiCoreB " << endl;
            return new MultiCoreB();
        };
};

int main()
{
    CoreFactoryB Fcb;
    SingleCore* core = Fcb.createSingleCore();
    MultiCore* Mcore = Fcb.createMultiCore();
    core->Show();
    Mcore->Show();
    CoreFactoryA Fca;
    core = Fca.createSingleCore();
    Mcore = Fca.createMultiCore();
    core->Show();
    Mcore->Show();
    return 0;
};