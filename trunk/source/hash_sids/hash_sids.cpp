#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <dtEntity/crc32.h>


int main (int argc, char *argv[])
{  
  if (argc < 4) 
  {
    printf("Usage: hash_sids infile outfile outfile_db");
    return 1;
  }
  
  std::ifstream instr(argv[1]);
  if(instr.fail()) 
  {
     std::cout << "Could not open input file " << argv[1] << std::endl;
     return 1;
  }
  


 
  typedef std::map<unsigned int, std::string> HashMap;
  HashMap hashed;

  {
     std::ifstream indbstr(argv[3]);
     if(!indbstr.fail()) 
     {
        while(indbstr.good() )
        {
           std::string line;
	        std::getline(indbstr, line);
           
           if(line.empty())
           {
              continue;
           }

           std::string::size_type offset = line.find_first_of(' ');           
           std::string hashstr = line.substr(0, offset);
           std::stringstream ss(hashstr);
           unsigned int hash; 
           ss >> hash;
           std::string text = line.substr(offset + 1, line.length() - 1);
           hashed[hash] = text;
        }
     }
  }

   
  std::cout << "CONVERTING " << argv[1] << " to " << argv[2] << "\n";
  const char* opentag = "dtEntity::SID(\"";
  const char* closetag = "\")";
  
  std::ostringstream os;
  char buffer[256];
  char sidcontents[256];
  bool found_sid = false;
  while(instr.good())
  {
    instr.getline(buffer, sizeof(buffer));

    char* buff_ptr = buffer;
    while(char* find_ptr = strstr(buff_ptr, opentag))
    {
      while(buff_ptr < find_ptr)
	   {
         os << *buff_ptr++;
	   }
      
      buff_ptr += strlen(opentag);
      int i;
      for(i = 0; i < 256; ++i)
      {
         if(*buff_ptr == '\"' && *(buff_ptr - 1) != '\\' && *(buff_ptr + 1) == ')')
         {
            sidcontents[i] = '\0';
            buff_ptr += strlen(closetag);
            break;
         }
         sidcontents[i] = *buff_ptr++;         
      }
      
      found_sid = true;

      unsigned int hash;
      MurmurHash3_x86_32(sidcontents, i, 0, &hash);
      os << hash;

      HashMap::iterator found = hashed.find(hash);
      if(found != hashed.end())
      {
         if(found->second != sidcontents)
         {
            std::cout << "Hash collision! Text1:" << std::endl;
            std::cout << "######################################" << std::endl;
            std::cout << sidcontents << std::endl;
            std::cout << "######################################" << std::endl;
            std::cout << "text2: " << std::endl;
            std::cout << found->second << std::endl;
            std::cout << "######################################" << std::endl;
         }
      }
      else
      {
         hashed[hash] = sidcontents;
      }   
    }
    os << buff_ptr << std::endl;
  }
  
  instr.close();

  if(!found_sid)
  {
     std::cout << "No call to SID function in " << argv[1] << std::endl;
     return 0;
  }

  std::ofstream outstr(argv[2]);
  if(outstr.fail())
  {
     std::cout << "Could not open output file " << argv[2] << std::endl;
     return 1;
  }

  outstr << os.str();

  outstr.close();



  std::ofstream outdbstr(argv[3]);
  if(outdbstr.fail()) 
  {
     std::cout << "Could not open output file " << argv[3] << std::endl;
     return 1;
  }

  for(HashMap::iterator i = hashed.begin(); i != hashed.end(); ++i)
  {
     outdbstr << i->first << " " << i->second << std::endl;
  }
  outdbstr.close();

  return 0;
}
