
class Loader
{
   private:
      int32_t lastAddress;  //last address stored to in memory
      bool loaded;          //set to true if .yo loaded into memory
      //helper methods for checking to make sure the
      //input file is properly formed and loading
      //the input file
      bool badFile(std::string);
      int32_t convert(std::string, int32_t, int32_t);
      void loadLine(std::string);
      bool hasErrors(std::string);
      bool hasAddress(std::string);
      bool hasData(std::string);
      bool hasComment(std::string);
      bool errorAddr(std::string);
      bool errorData(std::string, int32_t &);
      bool isSpaces(std::string, int32_t, int32_t);
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};
