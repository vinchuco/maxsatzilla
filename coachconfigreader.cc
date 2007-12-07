#include "coachconfigreader.hh"

CoachConfigReader::CoachConfigReader(const string &configFile) {

  ifstream file(configFile); 
  if(!file.is_open()) {
    cout << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }

  parseConfig(file);
  file.close();

}

CoachConfigReader::parseConfig(ifstream &file) {

  // For each loop we read a line in the file.
  while(file.good()) {
    char c;
    
    // First we read every space to the first character.
    while((c = file.get()) == ' ');
    
    // If it is a comment we read everything up to the end of file.
    if(c == 'c') {
      while(file.get() != '\n');
    }
    else if(c == 'p') {
      // Read everything up to the first non-space
      while(file.peek() == ' ') file.get();

      // Read string up to the next space
      string paramName;
      while(file.peek() != ' ') paramName += ((char)(file.get()));

      // Read everything up to the first non-space
      while(file.peek() == ' ') file.get();

    } else { // error
	  
    }
  }

}
