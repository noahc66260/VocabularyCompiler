// Quiz.cpp -- implementation of the Quiz class

#include"Quiz.h"
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<sstream>
#include<cctype>
#include<utility>

using namespace std;


void Quiz::cutWhiteSpace(string & str)
{
  unsigned int i;
  if (0 == str.size()) // empty string
    return;
  i = str.size()-1;
  while(isspace(str[i]) && i >= 0)
    i--;
  // erase whitespace at end of string
  str.erase(str.begin()+i+1, str.end());
  i = 0;
  if (str.size() == 0) // the entire string was whitespace
    return;
  while(isspace(str[i]) && i < str.length())
    i++;
  // erase whitespace at beginning of string
  str.erase(str.begin(),str.begin()+i);
}


void Quiz::storeQuiz1Answers(vector<info> & unshuffledList, 
                 vector<string> & shuffledDefinitions)
{
  quiz1Answers.clear(); // in case we are printing answers multiple times
  vector<string> multiple_entries; // for error checking
  bool counter;
  if (unshuffledList.size() != shuffledDefinitions.size())
  {
    cerr << "Quiz::storeQuiz1Answers(): "
      << "The two vector<info> arguments do not have the "
      << "same number of elements." << endl;
    return;
  }
  if (unshuffledList.size() != (unsigned) n)
  {
    cerr << "Quiz::storeQuiz1Answers(): "
      << "unshuffledList.size() != n.";
    return;
  }

  string answer;
  for (int i = 0; i < n; i++)
  {
    counter = false;
    for (int j = 0; j < n; j++)
    {
      if (unshuffledList[i].definition == shuffledDefinitions[j])
      {
      // for j >= 0 && j < n, we should only enter this if statement
      // once. Entering more than once would indicate that some words
      // in our input file have multiple definitions. 
        switch (j % 10) 
        {
          case 0: 
            answer = "a";
            break;
          case 1: 
            answer = "b";
            break;
          case 2: 
            answer = "c";
            break;
          case 3: 
            answer = "d";
            break;
          case 4: 
            answer = "e";
            break;
          case 5: 
            answer = "f";
            break;
          case 6: 
            answer = "g";
            break;
          case 7: 
            answer = "h";
            break;
          case 8: 
            answer = "i";
            break;
          case 9: 
            answer = "j";
            break;
          default:
            cerr << "Quiz::storeQuiz1Answers(): "
              << "we have erroneously reached the end "
              << "of the switch statment."
              << endl;
        }
        if (counter)
          multiple_entries.push_back(unshuffledList[i].definition);
        else
        {
          quiz1Answers.push_back(answer);
          counter = true;
        }
      }
    }
  }

  // we have enqueued all multiple entries in a vector.
  // We wish to display each definition that appears more than once
  // only once, so we sort our vector and eliminate identical
  // elements
  vector<string>::iterator it;
  sort(multiple_entries.begin(), multiple_entries.end());
  it = unique(multiple_entries.begin(), multiple_entries.end());
  multiple_entries.resize(it - multiple_entries.begin());

  // warn user of definitions with multiple entries
  if ( !multiple_entries.empty() )
  {
    cerr << "Multiple entries for the following definitions:" << endl;
    for (unsigned int i = 0; i < multiple_entries.size(); i++)
    {
      cerr << "\"" << multiple_entries[i] << "\"" << endl;
    }
  }
}

// the answers for quiz 2 are the words themselves
void Quiz::storeQuiz2Answers()
{
  // in case we are printing answers multiple times
  quiz2Answers.clear(); 
  for (unsigned int i = 0; i < selected_words.size(); i++)
    quiz2Answers.push_back(selected_words[i].word);
}

// the answers for quiz 3 are the definitions
void Quiz::storeQuiz3Answers()
{
  // in case we are printing answers multiple times
  quiz3Answers.clear(); 
  for (unsigned int i = 0; i < selected_words.size(); i++)
    quiz3Answers.push_back(selected_words[i].definition);
}


void Quiz::printAnswers(ostream & os, vector<string> & v, int columns, 
                        bool useparbox)
{
  double width = (1.0 - .05*columns) / columns;
  // for LaTeXHelper::parbox if useparbox == true

  // ensure columns argument is valid
  if (columns < MIN_COLUMNS || columns > MAX_COLUMNS)
  {
    cerr << "Quiz::printAnswers(): "
      << "columns field must be "
      << "greater than or equal to " << MIN_COLUMNS
      << " and less than or equal to " << MAX_COLUMNS
      << endl
      << "Using " << MIN_COLUMNS << "column(s)." << endl;
    columns = MIN_COLUMNS;
  }

  unsigned int i;
  os << latex.noIndent() << endl;
  os << latex.begin("align*");
  os << endl;
  for (i = 0; i < v.size(); i++)
  {
    os << latex.tab();
    os << (i + 1) << ".\\ & ";
/*
    if (1 == columns)
      os << latex.parbox(.9, "\\textwidth", v[i]);
    else
      os << latex.text(v[i]);
*/
    if (useparbox == true)
      os << latex.parbox(width, "\\textwidth", v[i]);
    else
      os << latex.text(v[i]);

    if (1 == columns)
      os << latex.newline();    
    else if ( i % columns == (unsigned) (columns - 1) && i != 0)
      os << latex.newline();
    else
      os << " & ";

    os << endl;
  }
  os << latex.end("align*");
  os << endl; 
}


void Quiz::prune(string word, string & definition)
{
  removeDelimitedText(definition, "(", ")");
  removeDelimitedText(definition, "[", "]");

  // convert definition to lowercase
  string lowercase_def;
  lowercase_def.resize(definition.size());
  transform(definition.begin(), definition.end(), 
    lowercase_def.begin(), ::tolower);
  transform(word.begin(), word.end(), word.begin(), ::tolower);

  // find instances of word and replace it with something else
  size_t found = lowercase_def.find(word);
  string before;
  string after;
  string replacement = pruneReplacement(word);
  while (string::npos != found)
  {
    // we've found a word that needs to be replaced
    before.assign(definition, 0, found);
    after.assign(definition, found + word.length(), definition.length());
    definition = before + replacement + after;
    lowercase_def.clear();
    lowercase_def.resize(definition.size());
    transform(definition.begin(), definition.end(), 
      lowercase_def.begin(), ::tolower);
    found = lowercase_def.find(word);
  }
}


string Quiz::pruneReplacement(string word)
{
  return string("\\underline{\\hspace{6 em}}");
}


void Quiz::removeDelimitedText(string & text, string left_delimiter, 
                               string right_delimiter)
{
/*
  This function removes delimited text only from the beginning
  of the text, so that the first nonblank character must be the
  left delimiter
*/
  size_t found;
  cutWhiteSpace(text);

  // check that left_delimiter is at the beginning of text
  if (text.find(left_delimiter) != 0)
    return;
  else // left_delimiter is at the beginning of text
  {
    found = text.find(right_delimiter);
    if (string::npos == found)
    {
      cerr << "Quiz::removeDelimitedText(): "
        << "Text string " << text << " does not contain "
        << "the right delimiter "
        << "\"" << right_delimiter << "\"" << endl;
      return;
    }
    else
      text.erase(0,found+1);
  }
}


void Quiz::removeDelimitedText(string & text, char left_delimiter, 
                               char right_delimiter)
{
  string ld = string(1, left_delimiter);
  string rd = string(1, right_delimiter);
  removeDelimitedText(text, ld, rd);
}


string Quiz::removeAlternateSpellings(string s)
{
  size_t found;
  found = s.find(" or ");
  if (found != string::npos) // multiple spellings exist
  {
    s.erase(s.begin()+found, s.end());
  }
  return s;
}


Quiz::Quiz(int n) : latex()
{
  if (n < 1)
  {
    cerr << "Quiz::Quiz(int): "
      << "Argument cannot be less than 1." 
      << endl
      << "Setting argument to " << DEFAULT_QUIZ_LENGTH 
      << endl;
    n = DEFAULT_QUIZ_LENGTH;
  }
  if (n > MAX_QUIZ_LENGTH)
  {
    cerr <<"Quiz::Quiz(int): "
      << "Argument cannot be greater than " << MAX_QUIZ_LENGTH 
      << "." << endl
      << "Setting argument to " << MAX_QUIZ_LENGTH
      << "." << endl;
    n = MAX_QUIZ_LENGTH;
  }
  this->n = n;
}


void Quiz::readFile(istream & is)
{
/*
  Text before the first colon is the word.
  Text after the first colon is the definition.
*/
  string w, def;
  info i;
  getline(is,w,':');
  getline(is,def);
  while (is.good())
  {
    cutWhiteSpace(w);
    i.word = w;
    cutWhiteSpace(def);
    i.definition = def;
    words.push_back(i);
    getline(is,w,':');
    getline(is,def);
  }
  is.clear(); // clear error flags
  selectWords();
}


// this is a multiple choice quiz
void Quiz::createQuiz1(ostream & os, bool newWords)
{
  if (true == newWords)
    selectWords();

  vector<string> definitions;
  vector<string> pruned_definitions;

  if  (true == newWords || quiz1Words.empty())
  {
    random_shuffle(selected_words.begin(), selected_words.end());

    // we store definitions and pruned definitions together
    vector< pair<string,string> > v;
    pair<string, string> p;
    for (unsigned int i = 0; i < selected_words.size(); i++)
    {
      p.first = selected_words[i].definition;
      p.second = p.first;
      prune(selected_words[i].word, p.second);
      v.push_back(p);
    }

    // we shuffle v and assign its contents to other vectors
    vector< pair<string,string> >::iterator iter = v.begin();
    for (unsigned int i = 0; i < v.size() / 10; i++, iter += 10)
      random_shuffle(iter, iter + 10);
    random_shuffle(iter, v.end());
    for (unsigned int i = 0; i < v.size(); i++)
    {
      definitions.push_back(v[i].first);
      pruned_definitions.push_back(v[i].second);
    }

    // store answers
    storeQuiz1Answers(selected_words, definitions);

    // we store our shuffled data in quiz1Words
    for (unsigned int j = 0; j < selected_words.size(); j++)
    {
      p.first = removeAlternateSpellings(selected_words[j].word);
      p.second = pruned_definitions[j];
      quiz1Words.push_back(p);
    }
  }

  // use the align* environment 
  unsigned int number = 0;
  char letter = 'a';
  os << latex.begin("align*") << endl;
  for (number = 0; number < selected_words.size(); number++, letter++)
  {
    // after every 10 words we print a horizontal line and create
    // appropriate spacing so the line doesn't obscure the words
    if (number != 0 && number % 10 == 0)
      os << latex.tab() << "\\hline "
        << latex.rule(0, string("ex"), 1.0, 
          string("\\baselineskip"), -.25, string("\\baselineskip"));

    // the multiple choice quiz only uses 10 entries at a time
    if (letter == 'a' + 10)
      letter = 'a';
    os << latex.tab()
      << latex.text(latex.intToString(number+1) 
        + string(". "))
      << " & " 
      << latex.parbox(20, "ex", quiz1Words[number].first)
      << " & "
      << endl
      << latex.tab()
      << latex.phantom("\\quad")
      << " & "
      << latex.text(letter + string(". "))
      << latex.parbox(.55,"\\textwidth", quiz1Words[number].second);

    // if the horizontal line is about to be printed we make
    // a little extra spacing for our newline
    if (number > 0 && (number + 1) % 10 == 0)
      os << latex.newline(.5, "ex");
    else
      os << latex.newline(); 

    os  << endl;
  }
  os << latex.end("align*") << endl;
}


// <definition> : <first letter of word>__________
// effectively fill in the blank
void Quiz::createQuiz2(ostream & os, bool newWords)
{
  if (true == newWords)
    selectWords();

  if  (true == newWords || quiz2Words.empty())
  {
    random_shuffle(selected_words.begin(), selected_words.end());
    storeQuiz2Answers();

    vector<string> pruned_definitions;
    for (unsigned int i = 0; i < selected_words.size(); i++)
    {
      pruned_definitions.push_back(selected_words[i].definition);
      prune(selected_words[i].word, pruned_definitions[i]);
    }

    // now we enqueue data into quiz2Words
    pair<string,string> p;
    for (unsigned int j = 0; j < selected_words.size(); j++)
    {
      p.first = selected_words[j].word;
      p.second = pruned_definitions[j];
      quiz2Words.push_back(p);
    }
  }

  // definition: <first letter of word>_________
  unsigned int number;
  os << latex.begin("align*") << endl;
  for (number = 0; number < selected_words.size(); number++)
  {
    os << latex.tab()
      << latex.text(latex.intToString(number+1) + string(". "))
      << " & "
      << latex.parbox(.60, "\\textwidth", quiz2Words[number].second)
      << " "
      << endl
      << latex.tab()
      << " & "
      << latex.text((quiz2Words[number].first)[0])
      << " & "
      << latex.horizontalLine(3, "cm")
      << latex.newline(.25,"ex")  
      << endl;
  }
  os << latex.end("align*") << endl;  
}


// Quiz 3 will be strictly <word>: ________ where the defintion must be guessed
void Quiz::createQuiz3(ostream & os, bool newWords)
{
  if (true == newWords)
    selectWords();

  if  (true == newWords || quiz3Words.empty())
  {
    random_shuffle(selected_words.begin(), selected_words.end());
    storeQuiz3Answers();

    pair<string, string> p;
    for (unsigned int j = 0; j < selected_words.size(); j++)
    {
      p.first = removeAlternateSpellings(selected_words[j].word);
      quiz3Words.push_back(p);
    }
  }

  // definition: <first letter of word>
  unsigned int number;
  os << latex.begin("align*") << endl;
  for (number = 0; number < selected_words.size(); number++)
  {
    os << latex.tab()
      << "& "
      << latex.text(latex.intToString(number+1) + string(". "))
      << latex.parbox(20, "ex", quiz3Words[number].first)
      << " "
      << endl
      << latex.tab() 
      << " & & "
      << latex.horizontalLine(6, "cm")
      << latex.newline(.5,"ex") 
      << endl;
  }
  os << latex.end("align*") << endl;  
}


void Quiz::selectNewWords(int num)
{
  int min_quiz_length = 1;
  if (num < min_quiz_length || n > MAX_QUIZ_LENGTH)
  {
    cerr << "Quiz::selectNewWords(int num): "
      << "Argument must be greater than or equal to "
      << min_quiz_length
      << " and less than or equal to " 
      << MAX_QUIZ_LENGTH
      << endl;
  }
  else if (words.empty())
  {
    cerr << "Quiz::selectNewWords(int num): "
      << "Words is empty. There are no words to select." 
      << endl;
  }
  else if ((unsigned) num > words.size())
  {
    cerr << "Quiz::selectNewWords(int num): "
      << "Argument cannot exceed the number of words available. "
      << "Setting n to " << words.size() << endl;
    n = words.size();
  }
  else // num is appropriate input
  {
    n = num;
    selectWords(); // since selectWords.size() != n anymore
  }
}


void Quiz::selectWords()
{
  selected_words.clear();
  quiz1Words.clear();
  quiz2Words.clear();
  quiz3Words.clear();
  random_shuffle(words.begin(), words.end());

  if ((unsigned) n > words.size())
  {
    cerr << "Quiz::selectWords(): "
      << "We cannot select " << n << " words because "
      << "there are only " << words.size() << " words available." 
      << endl
      << "Setting n to " << words.size() << endl;
    n = words.size();
  }
  selected_words.assign(words.begin(), words.begin() + n);
}
