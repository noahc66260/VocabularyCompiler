// quizCompiler.cpp -- creates vocab quizzes
// compile with Quiz.cpp and LaTeXHelper.cpp

#include<iostream>
#include"Quiz.h"
#include"LaTeXHelper.h"
#include<fstream>
#include<cctype>
#include<cstdlib>
#include<sstream>
#include<ctime>
#include<climits>
using namespace std;

// a repetition is a new sequence of up to three quizzes
const int MAX_REPETITIONS = 50;
const int MIN_REPETITIONS = 1;
const int DEFAULT_REPETITIONS = 1;
// the word number is the words per quiz
const int DEFAULT_WORD_NUMBER = 10;
const string DEFAULT_INPUT_FILE = "VocabList.txt";
// the master file is the concatenation/union of various input files
const string MASTER_FILE = "QuizCompilerInputFile.txt";
const string TEX_FILE = "VocabQuiz.tex";

union option
{
  bool counter;
  int value;
};

// precondition: none
// postconditions: flips the appropriate option switches and enqueues
// input file names in a vector of strings. The function returns
// the number of tokens processed as options.
int processOptions(int argc, char** argv, option opts[], vector<string> & v);

// precondition: ofs a valid stream
// postcondition: prints a formatted header in ofs
void printHeader(ofstream & ofs, string text, string sizeCommand);

// precondition:
// postcondition:
void printQuizzesAndAnswers(ofstream & ofs, Quiz & q, option opts[]);

// precondition: opts has its values initialized
// postcondition: displays on standard output what the option
// settings are as well as the specified input files.
void printOptions(option opts[], vector<string> & v);

// precondition: none
// postcondition: will return s as an int ("123" returns as 123)
// unless the string is empty, negative, or larger than MAX_INT,
// in which case it will return -1 
int stringToInt(string s);

// precondition: none
// postcondition: flips the corresponding switch to k (1 for quiz1,
// 2 for quiz2, and 3 for quiz3) and returns true or if k is not
// 1,2, or 3 it returns false
bool flipQuizSwitch(int k, option opts[]);

// precondition: s is a filename
// postcondition: returns true if s exists in the working directory,
// and false otherwise
bool fexists(string s);

// precondition: A file named s should not already exist
// postcondition: if v is empty, enqueues the default input file. 
// Then it extracts all values of v and concatenates them into
// a master file named s.
void createInputDocument(vector<string> & v, string s);

// precondition: s corresponds to a filename
// postcondition: returns true if each line in s has exactly one
// colon, and returns false otherwise.
bool checkFileFidelity(string s);

// precondition: none
// postcondition: prints the info/help menu on standard output
void printHelpMenu();

int main(int argc, char **argv)
{
  // necessary for random shuffling
  srand(time(NULL));

  option opts[8];
  vector<string> filenames;
  processOptions(argc, argv, opts, filenames);
  bool verbose = opts[0].counter;

  // if the user has not used the -q option
  if (!opts[1].counter && !opts[2].counter && !opts[3].counter)
  {
    opts[1].counter = true;
    opts[2].counter = true;
    opts[3].counter = true;
  }

  // if filenames is empty then I need to use default input file
  if (filenames.empty())
    filenames.push_back(string(DEFAULT_INPUT_FILE));

  if (verbose)
    printOptions(opts, filenames);

  // create our master file from input files
  createInputDocument(filenames, string(MASTER_FILE));

  if (verbose)
    cout << "Using " << MASTER_FILE << "." << endl;

  LaTeXHelper latex;
  ifstream ifs(string(MASTER_FILE).c_str());

  if (!ifs.is_open())
  {
    cerr << "Could not open " << MASTER_FILE << endl;
    exit(EXIT_FAILURE);
  }

  Quiz q(opts[7].value);
  q.readFile(ifs);

  // we clobber the old output file if it already exists
  ofstream ofs(string(TEX_FILE).c_str());
  if (!ofs.is_open())
  {
    cerr << "Could not open " << TEX_FILE << endl;
    exit(EXIT_FAILURE);
  } 

  ofs << latex.documentClass() << endl;
  latex.addPackage("amssymb");
  latex.addPackage("latexsym");
  latex.addPackage("fullpage");

  ofs << latex.usepackages() << endl;
  ofs << latex.begin("document") << endl << endl;
  ofs << latex.removePageNumbers() << endl;
  ofs << latex.allowDisplayBreaks() << endl;

  printQuizzesAndAnswers(ofs, q, opts);

  ofs << latex.end("document") << endl;

  ifs.close();
  ofs.close();

  // we compile our LaTeX file and reroute the output depending
  // on the whether or not the verbose option was chosen
  if (verbose)
    system(string("pdflatex " + TEX_FILE).c_str());
  else
    system(string("pdflatex " + TEX_FILE + " 1> /dev/null").c_str());

  // remove generated InputFile
  system(string("rm -f " + MASTER_FILE).c_str());
  return 0;
}


int processOptions(int argc, char** argv, option opts[], vector<string> & v)
{
  opts[0].counter = false; // verbose
  opts[1].counter = false; // quiz 1
  opts[2].counter = false; // quiz 2
  opts[3].counter = false; // quiz 3
  opts[4].counter = false; // new words for each quiz
  opts[5].counter = false; // new words for each repeat sequence
  opts[6].value = DEFAULT_REPETITIONS;     // number of repeat sequences
  opts[7].value = DEFAULT_WORD_NUMBER;     // number of words to select

  int i;
  string token;
  bool error = false;
  for(i = 1; i < argc; i++ )
  {
    token = argv[i];
    if (token[0] != '-') // all options must start with a hyphen
    {
      cerr << "Error: " << token << " is not an option" << endl;
      error = true;
    }
    else if (1 == token.length()) // a lone hyphen is not an option
    {
      cerr << "Error: lone hyphen" << endl;
      error = true;
    }
    else if (token == "--help")
    {
      if (i > 1)
      {
        cerr << "To get the help menu you must use the "
          << "--help option by itself" << endl;
      }
      else
      {
        printHelpMenu();
        exit(EXIT_SUCCESS);
      }
    }
    else if ("-v" == token) // verbose
    {
      opts[0].counter = true;
    }
    else if ("-x" == token) // don't choose new words for each sequence
    {
      opts[4].counter = true;
    }
    else if ("-r" == token.substr(0,2) && token.length() > 2)
    {
      string r = token;
      r.erase(r.begin(), r.begin()+2); // remove -r
      int k = stringToInt(r);

      // ensure valid int argument
      if (MIN_REPETITIONS > k || MAX_REPETITIONS < k)
      {
        cerr << "-r[int] option: In appropriate integer argument."
          << endl
          << "Argument must be between "
          << MIN_REPETITIONS
          << " and " 
          << MAX_REPETITIONS
          << "."
          << endl
          << "Setting option to -r" << MIN_REPETITIONS
          << endl;
        k = MIN_REPETITIONS;
      }
      opts[6].value = k;
    }
    else if ("-w" == token.substr(0,2) && token.length() > 2)
    {
      string r = token;
      r.erase(r.begin(), r.begin()+2); // remove -w
      int k = stringToInt(r);

      // ensure valid int argument
      if (1 > k)
      {
        cerr << "-w[int] option: In appropriate integer argument."
          << endl
          << "Argument must be a positive integer." 
          << endl
          << "Setting option to -w" 
          << DEFAULT_WORD_NUMBER
          << endl;
        k = DEFAULT_WORD_NUMBER;
      }
      opts[7].value = k;
    }
    else if ("-q" == token.substr(0,2))
    {
      token.erase(token.begin(), token.begin()+2); 
        // erase -q part of string
      size_t found;
      string substring;
      found = token.find_first_of(",");
      int k;
      bool b;

      // parse list and analyze substrings
      while (found != string::npos)
      {
        substring = token.substr(0, found);
        k = stringToInt(substring);
        b = flipQuizSwitch(k, opts);
        if (!b && substring.empty())
        { 
          cerr << "-q[list] option: Missing quiz number." << endl;
          error = true;
        }
        else if (!b)
        {
          cerr << "-q[list] option: " << substring << " is not "
            << "a valid quiz number" << endl;
          error = true;
        }
        token.erase(0,found+1); // lose the first arg + delimiter
        found = token.find_first_of(",");
      }
      substring = token;
      k = stringToInt(substring);
      b = flipQuizSwitch(k, opts);
      if (!b && substring.empty())
      { 
        cerr << "-q[list] option: Missing quiz number." << endl;
        error = true;
      }
      else if (!b)
      {
        cerr << "-q[list] option: " << substring << " is not "
            << "a valid quiz number" << endl;
        error = true;
      }
    }
    else if ("-f" == token)
    {
      i++; // we analyze [filename] following -f argument
      if (i >= argc) // no arguments follow
      {
        cerr << "-f [filename] option: "
          << "No filename following -f"
          << endl;
        error = true;
      }
      else
      {
        token = argv[i];
        if (!fexists(token))
        {
          cerr << "-f [filename] option: "
            << token << " is not a file" 
            << endl;
          error = true;
        }
        else
          v.push_back(token);
      }
    }
    else
    {
      cerr << "Error: " << token << " is not a valid option." << endl;
      error = true;
    }
  }
  if (error)
    exit(EXIT_FAILURE);
  else
    return i-1;
}

void printHeader(ofstream & ofs, string text, string sizeCommand)
{
  LaTeXHelper latex;
  ofs << latex.begin("center") 
    << endl
    << sizeCommand 
    << latex.textbold(text) 
    << endl
    << latex.end("center") 
    << endl;  
}


void printQuizzesAndAnswers(ofstream & ofs, Quiz & q, option opts[])
{
/*
  opts[0].counter -- verbose
  opts[1].counter -- quiz 1
  opts[2].counter -- quiz 2
  opts[3].counter -- quiz 3
  opts[4].counter -- new words for each quiz
  opts[5].counter -- new words for each repeat sequence
  opts[6].value   -- number of repeat sequences 
  opts[7].value    -- number of words per quiz
*/
  bool quiz1 = opts[1].counter;
  bool quiz2 = opts[2].counter;
  bool quiz3 = opts[3].counter;
  LaTeXHelper latex;
  for (int i = 0; i < opts[6].value; i++)
  { 
    // I need to select new words
    q.selectWords();

    // print newline so answers to previous repetitions are not on the 
    // same page as the next quiz
    if (0 < i) // we're not on the first repetition
      ofs << latex.newpage() << endl; 
    if (quiz1)
    {
      printHeader(ofs, "Quiz 1", "\\huge");
      ofs << latex.vfill() << endl;
      q.createQuiz1(ofs, opts[4].counter);
      ofs << latex.vfill() << endl;
      ofs << latex.newpage() << endl << endl;
    }
    if (quiz2)
    {
      printHeader(ofs, "Quiz 2", "\\huge");
      ofs << latex.vfill() << endl;
      q.createQuiz2(ofs, opts[4].counter);  
      ofs << latex.vfill() << endl;
      ofs << latex.newpage() << endl << endl;
    }
    if (quiz3)
    {
      printHeader(ofs, "Quiz 3", "\\huge");
      ofs << latex.vfill() << endl;
      q.createQuiz3(ofs, opts[4].counter);
      ofs << latex.vfill() << endl;
      ofs << latex.newpage() << endl << endl;
    }
    if (quiz1 && quiz2 && q.numberOfWords() <= 50)
    {
      ofs << latex.phantom("\\quad") << endl;
      ofs << latex.vfill() << endl;
      printHeader(ofs, "Quiz 1 Answers", "\\Large");
      ofs << "\\quad";
      ofs << latex.newline() << endl;
      q.listQuiz1Answers(ofs);
      ofs << latex.vfill() << endl;
      printHeader(ofs, "Quiz 2 Answers", "\\Large");
      ofs << "\\quad";
      ofs << latex.newline() << endl;
      q.listQuiz2Answers(ofs);    
      ofs << latex.vfill() << endl;
      ofs << latex.newpage() << endl << endl;
    }
    else  
    {
      if (quiz1 && q.numberOfWords() <= 50)
      {
        ofs << latex.phantom("\\quad") << endl;
        ofs << latex.vfill() << endl;
        printHeader(ofs, "Quiz 1 Answers", "\\Large");
        ofs << "\\quad";
        ofs << latex.newline() << endl;
        q.listQuiz1Answers(ofs);
        ofs << latex.vfill() << endl;
        ofs << latex.newpage() << endl << endl;
      }
      else if (quiz1 && q.numberOfWords() > 50)
      {
        printHeader(ofs, "Quiz 1 Answers", "\\Large");
        ofs << "\\quad";
        ofs << latex.newline() << endl;
        q.listQuiz1Answers(ofs);
        ofs << latex.newpage() << endl << endl;
      }

      if (quiz2 && q.numberOfWords() <= 50)
      {
        ofs << latex.phantom("\\quad") << endl;
        ofs << latex.vfill() << endl;
        printHeader(ofs, "Quiz 2 Answers", "\\Large");
        ofs << "\\quad";
        ofs << latex.newline() << endl;
        q.listQuiz2Answers(ofs);
        ofs << latex.vfill() << endl;
        ofs << latex.newpage() << endl << endl;
      }
      else if (quiz2 && q.numberOfWords() > 50)
      {
        printHeader(ofs, "Quiz 2 Answers", "\\Large");
        ofs << "\\quad";
        ofs << latex.newline() << endl;
        q.listQuiz2Answers(ofs);
        ofs << latex.newpage() << endl << endl;
      }
    }
    if (quiz3)
    {
      printHeader(ofs, "Quiz 3 Answers", "\\Large");
      ofs << "\\quad";
      ofs << latex.newline() << endl;
      q.listQuiz3Answers(ofs);
    }
  }
}

void printOptions(option opts[], vector<string> & v)
{
  bool verbose    = opts[0].counter;
  bool quiz1      = opts[1].counter;
  bool quiz2      = opts[2].counter;
  bool quiz3      = opts[3].counter;
  bool nwquiz     = opts[4].counter;
  bool nwsequence   = opts[5].counter;
  int nrepeat     = opts[6].value;
  int nwords      = opts[7].value;
  cout << "Option settings: " << endl
    << "Verbose = " << verbose << endl
    << "Quiz 1 = " << quiz1 << endl
    << "Quiz 2 = " << quiz2 << endl
    << "Quiz 3 = " << quiz3 << endl
    << "New words for each quiz = " << nwquiz << endl
    << "New words for each sequence = " << nwsequence << endl
    << "Repetitions = " << nrepeat << endl
    << "Number of words per quiz = " << nwords << endl;
  cout << "Our filenames are as follows: ";
  for (unsigned int i = 0; i < v.size(); i++)
    cout << endl << v[i];
  cout << endl;
}

int stringToInt(string s)
{
/*
  Will only process POSITIVE integers, returning -1 for an error
*/
  if (s.empty())
    return -1;

  // we check for nonnumerical input
  for (unsigned int j = 0; j < s.length(); j++)
    if (!isdigit(s[j]))
      return -1;

  // check to see that value is not larger than largest possible int
  if (atol(s.c_str()) > (long) INT_MAX)
  {
    cerr << "stringToInt(string): " 
      << "input " << s << " is greater than largest "
      << "possible integer value." << endl;
    return -1;
  }

  return atoi(s.c_str());
}

bool flipQuizSwitch(int k, option opts[])
{
  int Quiz1 = 1;
  int Quiz2 = 2;
  int Quiz3 = 3;

  if (Quiz1 == k)
  {
    opts[1].counter = true;
    return true;
  }
  else if (Quiz2 == k)
  {
    opts[2].counter = true;
    return true;
  }
  else if (Quiz3 == k)
  {
    opts[3].counter = true;
    return true;
  }
  else
    return false;
}

bool fexists(string s)
{
  ifstream ifs(s.c_str());
  if (ifs.good())
  {
    ifs.close();
    return true;
  }
  else
    return false;
}

void createInputDocument(vector<string> & v, string s)
{
  bool counter = false;

  if (v.empty())
    v.push_back(string(DEFAULT_INPUT_FILE));

  if (fexists(s.c_str()))
  {
    cerr << "createInputDocument(): "
      << s
      << " already exists."
      << endl;  
    exit(EXIT_FAILURE);
  }
  ofstream ofs(s.c_str());
  ifstream ifs;
  string line;
  for (unsigned int i = 0; i < v.size(); i++)
  {
    if (!fexists(v[i].c_str()))
    {
      cerr << "createInputDocument(): "
        << v[i] 
        << " is not a file"
        << " and will not be included in the master document." 
        << endl;
      continue;
    }
    // file exists
    checkFileFidelity(v[i]); // check file is in correct format
    ifs.open(v[i].c_str());
    getline(ifs, line);
    while(ifs.good())
    {
      ofs << line << endl;
      // endl is necessary since getline discards '\n' character
      getline(ifs, line);
    }
    ifs.close();
    counter = true; // we have at least one valid file
  }
  ofs.close();

  if (!counter)
  {
    cerr << "createInputDocument(vector<string>, string): "
      << "Function requires at least one valid input file."
      << endl;
    exit(EXIT_FAILURE);
  }
}

bool checkFileFidelity(string s)
{
  ifstream ifs;
  ifs.open(s.c_str());
  if (!ifs.good())
  {
    cerr << "checkFileFidelity(string): " 
      << s << " is not a file" << endl;
    ifs.close();
    return false;
  }
  string str;
  int n = 1; // line number
  getline(ifs, str);
  size_t e;

  // we store all erroneous lines in a vector
  vector< pair<int,string> > v;
  pair<int,string> p;
  while(ifs)
  {
    e = str.find(':', 0);
    // error if no colon exists on a line
    if (string::npos == e)
    {
      p.first = n;
      p.second = str;
      v.push_back(p);
    }
    else
    {   
      e = str.find(':', e+1);
      // error if more than one colon exists on a line
      if (string::npos != e)
      {
        p.first = n;
        p.second = str;
        v.push_back(p);
      }
    }
    getline(ifs,str);
    n++;
  }
  
  if (!v.empty())
  {
    cerr << "The file " << s << " contains lines that are problematic."
      << endl
      << "Each line should have exactly one colon between the word "
      << "and definition." << endl
      << "The following lines do not conform to this standard: "
      << endl;
    for (unsigned int i = 0; i < v.size(); i++)
      cerr << "line " 
        << v[i].first 
        << ": " 
        << v[i].second 
        << endl;
    return false;
  }
  else
    return true;
}

void printHelpMenu()
{
  cout << endl << 
    "Usage: ./[binary] OPTION..."
    << endl <<
    "Creates a LaTeX file from each file in input documents"
    << endl <<
    "There are no mandatory arguments as default values "
    << endl <<
    "will be given in the absence of explicit specification."
    << endl << endl << 
    "Optional Arguments:"
    << endl <<
    "-r[int]       Concatenate [int] number of copies, using different "
    << endl <<
    "              words for each sequence of quizzes"
    << endl <<
    "-x            Prevent each sequence from using different words"
    << endl <<
    "-w[int]        Assign [int] words to each quiz"
    << endl <<
    "-v            Print detailed output"
    << endl <<
    "-q[list]      Include selected quizzes. The default is to include"
    << endl <<
    "              all quizzes. Examples of usage:"
    << endl <<
    "              -q1 -q2 -q3   all three quizzes"
    << endl <<
    "              -q1,2,3       all three quizzes"
    << endl <<
    "              -q2 -q1       first two quizzes"
    << endl <<
    "              -q3,2         second and third quiz"
    << endl <<
    "              -q1           first quiz"
    << endl <<
    "-f [filename] Include words from file [filename]. This option"
    << endl <<
    "              may be used multiple times to include several"
    << endl <<
    "              files. "
    << endl << 
    "--help        Print the help menu."
    << endl << endl <<  
    "Options are not mutually exclusive and may be applied in any order."
    << endl <<
    "Options should not be combined, so -x and -v are valid options"
    << endl <<
    "but -xv is not a valid option."
    << endl << endl << 
    "The default settings are"
    << endl <<
    "-r1 -w10 -q1,2,3 -f VocabList.txt"
    << endl <<
    "Using an option overrides the default setting."
    << endl << endl << 
    "Output is a .tex file which will be compiled into a .pdf file "
    << endl <<
    "during runtime. Effort was made so that the output .pdf file "
    << endl <<
    "is formatted correctly, but it is impossible to know when "
    << endl <<
    "pagebreaks will occur so it is ultimately up to the user to " 
    << endl <<
    "either give input that formats well or to deal with poorly "
    << endl <<
    "formatted output. Generally large values for the -w[int] option "
    << endl <<
    "will produce unexpected pagebreaks. " 
    << endl << endl;
}
