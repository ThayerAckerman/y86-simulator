//class to test the RegisterFile class
class RegisterFileTester
{
   private:
      void doClearRegisterTests();
      void doReadWriteRegisterTests();
      void doReadWriteErrorTests();
      RegisterFile * rf;
   public:
      RegisterFileTester();
      void doRegisterFileTests();
};

