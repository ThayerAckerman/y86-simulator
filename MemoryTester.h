//class to test the Memory class
class MemoryTester
{
   private:
      void doClearMemoryTests();
      void doPutGetByteTests();
      void doPutGetLongTests();
      void doByteErrorTests();
      void doLongErrorTests();
      Memory * mem;
   public:
      MemoryTester();
      void doMemoryTests();
};

