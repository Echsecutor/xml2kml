/**
 * @file xml2kml.cpp
 * @author Sebastian Schmittner <sebastian@schmittner.pw>
 *
 * @section DESCRIPTION
 *
 * Parse http://nominatim.openstreetmap.org search query result, such as
 * http://nominatim.openstreetmap.org/search.php?q=Spielplatz+Cologne&format=xml&limit=100
 * and convert to KML (list of places).
 *
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <fstream>

#include <string>
#include <string.h>


using namespace std;


void help(){
  cout << "xml2kml [-h] [-n NAME] INPUT.xml [[-o] OUTPUT.kml]"<<endl;
  cout << "Parse nominatim xml search query result, e.g."<<endl;
  cout <<"http://nominatim.openstreetmap.org/search.php?q=Spielplatz+Cologne&format=xml&limit=100" <<endl;
  cout << "and convert the result list to a KML file."<<endl;
  cout <<endl;
  cout << "-h\tdisplay this text"<<endl;
  cout << "-n\tset NAME as the name attribute of the kml places"<<endl;
  cout << "if -o is omitted, the OUTPUT file name has to come after the INPUT."<<endl;
}


int main(int args, char *arg[]){

  char * infile=0;
  char * outfileArg=0;
  char * useName=0;

  for (int i=1;i<args;i++){
    if(strcmp(arg[i],"-h")==0||strcmp(arg[1],"--help")==0){
      help();return 0;
    }else if(strcmp(arg[i],"-n")==0){
      if(args<i+1){help();return 0;}
      useName= arg[i+1];
      i++;
    }else if(strcmp(arg[i],"-o")==0){
      if(args<i+1){help();return 0;}
      outfileArg = arg[i+1];
      i++;
    }else if(infile==0){
      infile = arg[i];
    }else{
      outfileArg = arg[i];
    }
  }

  if(infile==0){help();return 0;}

  string name="Search Result";
  if(useName!=0){
    name=useName;
  }

  string outfile="output.kml";
  if(outfileArg!=0){
    outfile=outfileArg;
  }

  ifstream in(infile);
  ofstream out(outfile.c_str());
  string line;

  if (!in.good() || !out.good()){
    cerr<< "Error opening files."<<endl;
    return(1);
  }



  //write header
  out<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
  out << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">"<<endl;

  bool place=true;
  int count=0;
  cout << "Reading input from " <<infile <<endl;

  while(getline(in,line,'>')){
    cout << "Found xml tag"<<endl;
    place=true;
    int start = line.find("<place");
    if(start<0){
      place=false;
      cout << "Not a <place"<<endl;
    }else{
      int latStart = line.find("lat=\"",start);
      int latStop = line.find("\"",latStart + 5);
      if(latStart<0 || latStop<0){
	cerr << "Truncated input!"<<endl;
	place=false;
      }
      string lat = line.substr(latStart + 5, latStop - latStart -5);

      int lonStart = line.find("lon=\"",start);
      int lonStop = line.find("\"",lonStart + 5);
      if(lonStart<0 || lonStop<0){
	cerr << "Truncated input!"<<endl;
	place=false;
      }
      string lon = line.substr(lonStart + 5,  lonStop - lonStart - 5);

      int descStart = line.find("display_name=\"",start);
      int descStop = line.find("\"",descStart + 14);
      if(descStart<0 || descStop<0){
	cerr << "Truncated input!"<<endl;
	place=false;
      }
      string desc = line.substr(descStart + 14, descStop - descStart - 14);

      if(place){
	count++;
	cout << "Converting search result number "<< count <<endl;
	out << "<Placemark>"<<endl;
    	out << "<name>"<< name <<"</name>"<<endl;
    	out << "<description>"<< desc <<"</description>"<<endl;
    	out << "<Point>"<<endl;
      	out << "<coordinates>" << lon << "," << lat << "," << "0</coordinates>"<<endl;
    	out << "</Point>"<<endl;
  	out << "</Placemark>"<<endl;
      }

    }//endif found new result
  }//wend


  out <<"</kml>"<<endl;

  out.close();
  in.close();

  cout << "done. Result written to " <<outfile<<endl;


  return 0;
}
