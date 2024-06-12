#include <string>

class User
{
    private:
        std::string buttonPattern;
        int fingerprintId;
        std::string verbalPassword;
    
    public:
        std::string name;
        bool verifyButtonPattern(std::string code) {return code == buttonPattern;};
        bool verifyFingerPrint(int id) {return id == fingerprintId;};
        bool verifyVerbalPassword(std::string password) {return password == verbalPassword;};
        void setButtonPattern(std::string code) {buttonPattern = code;};
        void setFingerPrintId(int id) {fingerprintId = id;};
        void setVerbalPassword(std::string password) {verbalPassword = password;};
        bool compareUsers(User &user) {return buttonPattern == user.buttonPattern && fingerprintId == user.fingerprintId && verbalPassword == user.verbalPassword;};

        User() {
            name = "NEW USER";
            buttonPattern = "";
            fingerprintId = 0;
            verbalPassword = "";
        }
    
};