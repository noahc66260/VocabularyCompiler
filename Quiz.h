// Quiz.h -- header for the Quiz class

#ifndef QUIZ_H_
#define QUIZ_H_

#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include"LaTeXHelper.h"

using namespace std;

/*
  Maybe I should make a typedef
  typedef vector<pair<string,string>> info

  It turns out it wouldn't be too difficult to create a fourth
  kind of quiz that poses statements with the word which
  you must determine to be true or false. 
  Hm... the additional programming that would have to go into this
  to make everything compatible with multiple kinds of input
  files as well as specifying which kind of input file we would
  like to use would indeed be a big project. Since this is not
  a commercial project I will leave the note here and may or may
  not decide to add in that functionality in the future. 
*/

struct info
{
  string word;
  string definition;
};

// This is an option that may or may not be useful in the future
// as another upper bound is the number of words available from
// the input files
const int MAX_QUIZ_LENGTH = 1000;
const int DEFAULT_QUIZ_LENGTH = 10;
const int MAX_COLUMNS = 10;
const int MIN_COLUMNS = 1;

/*
  Class invariants:
    n >= 0
*/

class Quiz
{
private:
  int n; // the number of questions to include in the quiz
  LaTeXHelper latex;
  vector<info> words;
  vector<info> selected_words;
  vector< pair<string, string> > quiz1Words;
  vector< pair<string, string> > quiz2Words;
  vector< pair<string, string> > quiz3Words;
  vector<string> quiz1Answers;
  vector<string> quiz2Answers;
  vector<string> quiz3Answers;

  // Description: removes white space surrounding str
  // Precondition: none
  // Postcondition: str is modified
  void cutWhiteSpace(string & str);

  // Description: returns the word member of an info struct
  // Precondition: none
  // Postcondition: returns the word member of i
  string word(info & i) {return i.word;}

  // Description: returns the definition member of an info struct
  // Precondition: none
  // Postcondition: returns the definition member of i
  string definition(info & i) {return i.definition;}

  // Description: stores quiz 1 answers in quiz1Answers object
  // Precondition: unshuffledList.size() == shuffledDefinitions.size() == n
  // Postcondition: quiz1Answers.size() == unshuffledList.siz()
  void storeQuiz1Answers(vector<info> & unshuffledList, 
                  vector<string> & shuffledDefinitions);

  // Description: stores quiz 2 answers in quiz2Answers object
  // Precondition: word members in selectedWords[i] info structs
  // for all i >= 0 && i < selectedWords.size() should reflect
  // entries in quiz2Words object
  // Postcondition: quiz2Answers.size() == selectedWords.size()
  void storeQuiz2Answers();

  // Description: stores quiz 3 answers in quiz3Answers object
  // Precondition: definition members in selectedWords[i] info structs
  // for all i >= 0 && i < selectedWords.size() should reflect
  // entries in quiz3Words object
  // Postcondition: quiz3Answers.size() == selectedWords.size()
  void storeQuiz3Answers();

  // Description: output to os object is LaTeX commands for text
  // in the aligned* environment with columns columns. Each entry
  // is a number followed by a period and a string from v.
  // Precondition: columns >= MIN_COLUMNS && columns <= MAX_COLUMNS
  // Postcondition: output to os is a valid LaTeX command
  void printAnswers(ostream & os, vector<string> & v, int columns, 
    bool useparbox = false);

  // Description: modifies definition so that any instances of word
  // that appear in the actual text of definition are replaced
  // with an underline
  // Precondition: none
  // Postcondition: definition is modified and does not contain any
  // instances of word
  void prune(string word, string & definition);

  // Description: returns the replacement of word we should use
  // in the function prune()
  // Precondition: none
  // Postcondition: returns a replacement string
  string pruneReplacement(string word);

  // Description: if the text starts with left_delimiter,
  // the function removes all text up to and including
  // right_delimiter. The delimiter may be more than one character.
  // Precondition: none
  // Postcondition: text is modified if the delimiters were found
  // no changes are made to text if the delimiters are not found
  void removeDelimitedText(string & text, string left_delimiter, 
                           string right_delimiter);

  // Description: if the first character in text is left_delimiter,
  // the function removes all text up to and including
  // right_delimiter. This version will accept char arguments
  // for the delimiters
  // Precondition: none
  // Postcondition: text is modified if the delimiters were found
  // no changes are made to text if the delimiters are not found
  void removeDelimitedText(string & text, char left_delimiter, 
                           char right_delimiter);

  // Description: if a word is comprised of alternate spellings
  // in the form spelling1 or spelling2 or ...
  // the function will return spelling1
  // Precondition: none
  // Postcondition: returns the first spelling of the word
  string removeAlternateSpellings(string s);

public:

  // Description: constructor for Quiz class
  // Precondition: n >= 0 && n <= MAX_QUIZ_LENGTH
  // Postcondition: creates Quiz object
  Quiz(int n = DEFAULT_QUIZ_LENGTH);

  // Description: reads is object and adds words and definitions
  // to words object
  // Precondition: is.good() == true 
  // Postcondition: words.size() == number of lines in istream object
  void readFile(istream & is);

  // Description: prints LaTeX formatting of quiz 1 to os. If
  // newWords == true then we select new words before creating
  // the quiz. This function also calls storeQuiz1Answers()
  // Precondition: os.good() == true
  // Postcondition: quiz1Words.size() == n && n == quiz1Answers.size()
  void createQuiz1(ostream & os, bool newWords = false);

  // Description: prints LaTeX formatting of quiz 2 to os. If
  // newWords == true then we select new words before creating
  // the quiz. This function also calls storeQuiz2Answers()
  // Precondition: os.good() == true
  // Postcondition: quiz2Words.size() == n && n == quiz2Answers.size()
  void createQuiz2(ostream & os, bool newWords = false);

  // Description: prints LaTeX formatting of quiz 3 to os. If
  // newWords == true then we select new words before creating
  // the quiz. This function also calls storeQuiz3Answers()
  // Precondition: os.good() == true
  // Postcondition: quiz3Words.size() == n && n == quiz3Answers.size()
  void createQuiz3(ostream & os, bool newWords = false);

  // Description: We use the words object to find num new words
  // for our selectedWords object
  // Precondition: num >= 0
  // Postcondition: selectedWords.size() == num, n = num
  void selectNewWords(int num);

  // Description: We use the words object to find n new words
  // for our selectedWords object
  // Precondition: none
  // Postcondition: selectedWords.size() == n
  void selectWords();

  // Description: returns value of member n
  // Precondition: object is initialized
  // Postcondition: none
  int numberOfWords() {return n;}

  // Description: prints quiz 1 answers to os with the align* environment
  // Precondition: os.good() == true
  // Postcondition: none
  void listQuiz1Answers(ostream & os)
    {printAnswers(os, quiz1Answers, 10, false);}

  // Description: prints quiz 2 answers to os with the align* environment
  // Precondition: os.good() == true
  // Postcondition: none
  void listQuiz2Answers(ostream & os)
    {printAnswers(os, quiz2Answers, 2, true);}

  // Description: prints quiz 2 answers to os with the align* environment
  // Precondition: os.good() == true
  // Postcondition: none
  void listQuiz3Answers(ostream & os)
    {printAnswers(os, quiz3Answers, 1, true);}

  // Description: class destructor
  // Precondition: none
  // Postcondition: none
  ~Quiz() {}
};

#endif

