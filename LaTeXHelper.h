// LaTeXHelper.h -- interface for LaTeXHelper class

#ifndef LATEXHELPER_H_
#define LATEXHELPER_H_

#include<iostream>
#include<list>
#include<vector>
using namespace std;

/*
  Class invariants:
    level >= 0
    size >= 0
*/

class LaTeXHelper
{
private:
  string documentclass;
  int size; // size of text
  int level; // the size of a tab in spaces for the .tex document
  list<string> packages;
  vector<string> acceptableLengthUnits;

  // Description: checks if units is an appropriate LaTeX length unit
  // Precondition: none
  // Postcondition: if units is not an appropriate LaTeX length unit,
  // an error message will be printed for the user.
  void checkLengthUnits(string units);

public:
  // Description: class constructor
  // Precondition: none
  // Postcondition: acceptableLengthUnits, size, and level
  // are initialized
  LaTeXHelper();

  // Description: class destructor
  // Precondition: none
  // Postcondition: object destroyed
  ~LaTeXHelper() {}

  // Description: prints a tab (a sequence of whitespaces)
  // Precondition: level is initialized and level >= 0
  // Postcondition: returns a sequence of whitespaces as a string
  string tab();

  // Description: typesets the beginning of an environment in LaTeX
  // Precondition: level is initialized
  // Postcondition: returns a string that can be used in a .tex doc
  string begin(string environment)
    {level++; return "\\begin{" + environment + "}";}

  // Description: typesets the end of an environment in LaTeX
  // Precondition: level is initialized
  // Postcondition: returns a string that can be used in a .tex doc
  string end(string environment)
    {level--; return "\\end{" + environment + "}";}

  // Description: adds a package to packages variable
  // Precondition: none
  // Postcondition: packages has one more element
  void addPackage(string package)
    {packages.push_front(package);}

  // Description: typesets the \documentclass{...} command in LaTeX
  // Precondition: size is initialized, documentclass is initialized
  // Postcondition: returns the command in string form
  string documentClass();

  // Description: typesets the \usepackages{...,...} command in LaTeX
  // Precondition: none
  // Postcondition: returns a string of various usepackage{} commands
  string usepackages();

  // Description: gives the LaTeX command that allows page breaks
  // Precondition: none
  // Postcondition: returns a string with the appropriate command
  string allowDisplayBreaks()
    {return string("\\allowdisplaybreaks");}

  // Description: 
  // Precondition: none
  // Postcondition:
  string removePageNumbers()
    {return string("\\thispagestyle{empty}\n\\pagestyle{empty}");}

  // Description: reset size of font to n
  // Precondition: n >= 0
  // Postcondition: size = n
  void setSize(int n);

  // Description: reset document class to text
  // Precondition: text is a valid document class
  // Postcondition: documentclass = text
  void setDocumentClass(string text)
    {documentclass = text;}

  // Description: use for \newpage LaTeX command
  // Precondition: none
  // Postcondition: returns the command as a string
  string newpage()
    {return "\\newpage";}

  // Description: use for \clearpage LaTeX command
  // Precondition: none
  // Postcondition: returns the command as a string
  string clearpage()
    {return "\\clearpage";}

  // Description: converts an int into a string
  // Precondition: none
  // Postcondition: returns the int in string form
  string intToString(int n) const;

  // Description: converts a double into a string
  // Precondition: none
  // Postcondition: returns the double in string form
  string doubleToString(double n) const;

  // Description: use for \parbox LaTeX command
  // Precondition: width > 0, units is valid LaTeX length unit,
  // text does not contain special unescaped LaTeX characters
  // (the underscore, for example)
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command
  string parbox(double width, string units, string text) const;

  // Description: use for \newline LaTeX command
  // Precondition: units is valid LaTeX length unit
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command "\\newline[n units]"
  string newline(double n = 0, string units = "ex");

  // Description: use for \hspace LaTeX command
  // Precondition: units is valid LaTeX length unit
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command "\\hspace[n units]"
  string hspace(double n, string units);

  // Description: use for \vspace LaTeX command
  // Precondition: units is valid LaTeX length unit
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command "\\vspace[n units]"
  string vspace(double n, string units);

  // Description: a sobriquet for the hline() function
  // Precondition: n > 0, units is valid LaTeX length unit
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command "\\hline[n units]"
  string horizontalLine (double n, string units)
    {return hline(n, units);}

  // Description: use for \hline LaTeX command
  // Precondition: n > 0, units is valid LaTeX length unit
  // Postcondition: returns the command with arguments of function
  // as arguments of LaTeX command "\\hline[n units]"
  string hline(double n, string units)
    {return string("\\underline{") + hspace(n, units) + string("}");}

  // Description: use for \text LaTeX command
  // Precondition: text does not contain special unescaped 
  // LaTeX characters (the underscore, for example)
  // Postcondition: returns the command as a string
  string text(string s)
    {return string("\\text{") + s + string("} ");}

  // Description: use for the \text LaTeX command
  // Precondition: c does not contain special unescaped 
  // LaTeX characters (the underscore, for example)
  // Postcondition: returns the command as a string
  string text(const char c)
    {return text(string(1,c));}

  // Description: use for the \textbf LaTeX command
  // Precondition: text does not contain special unescaped 
  // LaTeX characters (the underscore, for example)
  // Postcondition: returns the command as a string
  string textbold(string s)
    {return string("\\textbf{") + s + string("} ");}

  // Description: use for the \textbf LaTeX command
  // Precondition: c does not contain special unescaped 
  // LaTeX characters (the underscore, for example)
  // Postcondition: returns the command as a string
  string textbold(const char c)
    {return textbold(string(1,c));}

  // Description: use for the \rule LaTeX command
  // Precondition: width > 0, widthUnits is a valid LaTeX length unit, 
  // height > 0, heightUnits is a valid LaTeX length unit, 
  // offsetUnits is a valid LaTeX length unit
  // Postcondition: returns the command as a string
  string rule(double width, string widthUnits, double height, 
              string heightUnits, double offset = 0.0, 
              string offsetUnits = "cm");

  // Description: prints a strut using the rule() function
  // Precondition: none
  // Postcondition: returns a LaTeX command as a string
  string strut()
    {return rule(0.0, string("ex"), 1.0, string("\\baselineskip"), 
                 -.3, string("\\baselineskip"));}

  // Description: use for the \vfill LaTeX command
  // Precondition: none
  // Postcondition: returns the command as a string
  string vfill()
    {return string("\\vfill");}

  // Description: use for the \phantomLaTeX command
  // Precondition: text does not contain special unescaped 
  // LaTeX characters (the underscore, for example)
  // Postcondition: returns the command as a string
  string phantom(string text)
    {return string("\\phantom{") + text + string("}");}

  // Description: use for the \noindent LaTeX command
  // Precondition: none
  // Postcondition: returns the command as a string
  string noIndent()
    {return string("\\noindent");}
};

#endif
