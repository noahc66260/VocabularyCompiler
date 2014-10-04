// LaTeXHelper.cpp -- implementation of the LaTeXHelper class

#include"LaTeXHelper.h"
#include<iostream>
#include<cstdlib>
#include<sstream>
#include<algorithm>
using namespace std;

void LaTeXHelper::checkLengthUnits(string units)
{
  vector<string>::iterator it;
  it = find(acceptableLengthUnits.begin(), 
        acceptableLengthUnits.end(), units);
  if (acceptableLengthUnits.end() == it)
  {
    cerr << "LaTeXHelper::checkLengthUnits(string units): "
      << units << " is not a length unit. " 
      << endl
      << "Acceptable length units are as follows." 
      << endl;
    unsigned int i;
    for (i = 0; i < acceptableLengthUnits.size() - 1; i++)
    {
      cerr << acceptableLengthUnits[i] << ", "; 
    }
    cerr << acceptableLengthUnits[i] << endl;
  }
}


LaTeXHelper::LaTeXHelper() 
{
  level = 0; 
  size = 10; 
  documentclass = "amsart";
  acceptableLengthUnits.push_back("in");
  acceptableLengthUnits.push_back("mm");
  acceptableLengthUnits.push_back("cm");
  acceptableLengthUnits.push_back("pt");
  acceptableLengthUnits.push_back("em");
  acceptableLengthUnits.push_back("ex");
  acceptableLengthUnits.push_back("pc");
  acceptableLengthUnits.push_back("bp");
  acceptableLengthUnits.push_back("dd");
  acceptableLengthUnits.push_back("cc");
  acceptableLengthUnits.push_back("\\baselineskip");
  acceptableLengthUnits.push_back("\\baselinestretch");
  acceptableLengthUnits.push_back("\\columnsep");
  acceptableLengthUnits.push_back("\\columnwidth");
  acceptableLengthUnits.push_back("\\evensidemargin");
  acceptableLengthUnits.push_back("\\linewidth");
  acceptableLengthUnits.push_back("\\oddsidemargin");
  acceptableLengthUnits.push_back("\\paperwidth");
  acceptableLengthUnits.push_back("\\paperheight");
  acceptableLengthUnits.push_back("\\parindent");
  acceptableLengthUnits.push_back("\\parskip");
  acceptableLengthUnits.push_back("\\tabcolsep");
  acceptableLengthUnits.push_back("\\textheight");
  acceptableLengthUnits.push_back("\\textwidth");
  acceptableLengthUnits.push_back("\\topmargin");
  acceptableLengthUnits.push_back("\\unitlength");
}


string LaTeXHelper::tab()
{
  if (level < 0)
    level = 0;
  return string(level * 3, ' ');
}


string LaTeXHelper::documentClass()
{
  string textsize = intToString(size);
  return string("\\documentclass[") 
    + textsize
    + string(" pt]{") 
    + documentclass 
    + string("}");  
}


string LaTeXHelper::usepackages()
{
/*
  out should be
  \usepackage{...}
  \usepackage{...}
  \usepackage{...}
  ...
*/
  string line;
  while (!packages.empty())
  {
    line += "\\usepackage{";
    line += packages.front();
    line += "}\n";
    packages.pop_front();
  }
  return line;
}


void LaTeXHelper::setSize(int n)
{
  if (n > 0)
    size = n;
  else
  {
    cerr << "Argument to size must be a positive integer" << endl;
  }
}


string LaTeXHelper::intToString(int n) const
{
  stringstream out;
  out << n;
  return out.str(); 
}


string LaTeXHelper::doubleToString(double n) const
{
  stringstream out;
  out << n;
  return out.str(); 
}


string LaTeXHelper::parbox(double width, string units, string text) const
{
  if (width <= 0)
  {
    cerr << "LaTeXHelper::parbox(double, string, string): "
      << "First argument must be a nonnegative number." 
      << endl;
    exit(EXIT_FAILURE);
  }
  return string("\\parbox[t]{") 
      + doubleToString(width) 
      + string(" " + units + "}{") 
      + text 
      + string("}");
}


string LaTeXHelper::newline(double n, string units)
{
  checkLengthUnits(units);
  return "\\\\[" + doubleToString(n) + " " + units + "]";
}


string LaTeXHelper::hspace(double n, string units)
{
  checkLengthUnits(units);
  return string("\\hspace{") + doubleToString(n) + " " + units + "}";
}


string LaTeXHelper::vspace(double n, string units)
{
  checkLengthUnits(units);
  return string("\\vspace{") + doubleToString(n) + " " + units + "}";
}


string LaTeXHelper::rule(double width, string widthUnits, double height, 
                         string heightUnits, double offset, string offsetUnits)
{
  checkLengthUnits(widthUnits);
  checkLengthUnits(heightUnits);
  checkLengthUnits(offsetUnits);

  string stringWidth = doubleToString(width);
  string stringHeight = doubleToString(height);
  string stringOffset = doubleToString(offset);

  return "\\rule[" + stringOffset + offsetUnits + "]{"
      + stringWidth + widthUnits + "}{"
      + stringHeight + heightUnits + "}";
}
