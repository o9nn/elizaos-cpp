#include "constants.hpp"
#include <string>

string SERVER_URL = std::string("http://localhost:7998");
string SUPABASE_URL = std::string("https://pronvzrzfwsptkojvudd.supabase.co");
string SUPABASE_ANON_KEY = std::string("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InByb252enJ6ZndzcHRrb2p2dWRkIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MDY4NTYwNDcsImV4cCI6MjAyMjQzMjA0N30.I6_-XrqssUb2SWYg5DjsUqSodNS3_RPoET3-aPdqywM");
string TEST_EMAIL = std::string("testuser123@gmail.com");
string TEST_PASSWORD = std::string("testuser123@gmail.com");
string TEST_EMAIL_2 = std::string("testuser234@gmail.com");
string TEST_PASSWORD_2 = std::string("testuser234@gmail.com");
any zeroUuid = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));

void Main(void)
{
}

MAIN
