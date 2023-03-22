///**-------------------------------------------
//
//Project 1 : Gerrymandering
//
//Course: CS 251, Spring 2022, UIC
//
//Author: Sanskar Kalal(651632119)
//  
//**--------------------------------------------

#include <fstream>
#include "iomanip"
#include "iostream"
#include "ourvector.h"
#include "string"
#include "cmath"
using namespace std;

// create a struct of type district to store the votes of each state
struct district{
    int districtId;
    int demVotes;
    int repVotes;
};

/* create a struct of type state that holds the name, eligible voters
 * and a vector of type district which will contain
 * data for each district
*/
struct state{
    string name;  //name of the state
    int elegibleVoters = 0; // number of eligible voters initialized to 0
    ourvector<district> districts; // a vector of all the districts in a state
};

// helper funtion to lower any string that is passed through it
string lower(const string& pass){
    string lowerStr;
    for(char pas : pass){
        lowerStr+= tolower(pas);
    }
    return lowerStr;
}


// helper function to split the string by delimeter and stores the seperated elements in a vector
void splitString(const string& line, char delimiter, ourvector<string>& items ) {
    stringstream s_stream(line);
    while (s_stream.good()) {
        string item;
        getline(s_stream, item, delimiter);
        items.push_back(item);
    }
}


// loads the districts data into our state struct
void loadToStruct(ourvector<string> &temp,ourvector<state> &stateList){
    ourvector<string> elements;
    state stateData;
    district districtData;

    // loop through each line
    for (int i = 0; i < temp.size(); ++i) {
        // split the line at commas to get each element
        splitString(temp.at(i),',',elements);

        // first element is the name of the state
        stateData.name = elements.at(0);

        // looping every third element
        for (int j = 1; j < elements.size(); j+=3) {

            if(elements.at(j)=="AL"){
                elements.at(j)= "1";
            }
            districtData.districtId = stoi(elements.at(j)); // the district number
            districtData.demVotes = stoi(elements.at(j+1)); // the democratic votes
            districtData.repVotes = stoi(elements.at(j+2)); // the republic votes

            stateData.districts.push_back(districtData); // push the data back into the vector of districts


        }
        cout<<"..."<<stateData.name<<"..."<<stateData.districts.size()<<" districts total"<<endl;

        stateList.push_back(stateData); // push the whole state data back into the stateList
        stateData.districts.clear();  // clear the vector for the next line
        elements.clear(); // clear the elements for the next line
    }
    cout<<endl;
}


// reads the district file and fills it up into the stateList vector
void loadDistFile(string file,ourvector<state> &stateList,bool &districtFileLoaded){
    ourvector<string> temp;
    string line;
    ifstream myfile(file);

    // check if file is valid
    if (!myfile.is_open())
    {
        districtFileLoaded = false;
        cout<<"Invalid first file, try again."<<endl;
        cout<<endl;
    }
    if (myfile.is_open())
    {
        districtFileLoaded = true;
        cout<< "Reading: "<<file<<endl;
        while ( getline (myfile,line) )
        {
            // push each line
            temp.push_back(line);
        }
        myfile.close();
        loadToStruct(temp,stateList);

    }

}




// loads the voter file which fills the eligible voters in the struct
void loadVoterFile(const string& file,ourvector<state>&stateList,bool &voterFileLoaded){
    ourvector<string> temp;

    string line;
    ifstream myfile(file);
    if (!myfile.is_open())
    {
        voterFileLoaded = false;
        cout<<"Invalid second file, try again."<<endl;
        cout<<endl;
    }
    // reads the file
    if (myfile.is_open())
    {
        voterFileLoaded = true;
        cout<< "Reading: "<<file<<endl;
        while ( getline (myfile,line) )
        {
            // splitting each line and storing it in temp vector
            splitString(line,',',temp);
        }
        myfile.close();

        // loop through the vector and fill the voters in the struct
        for (int i = 0; i < temp.size(); ++i) {
            for (int j = 0; j < stateList.size(); ++j) {
                if(temp.at(i) == stateList.at(j).name){
                    stateList.at(j).elegibleVoters = stoi(temp.at(i+1));
                    cout<<"..."<<stateList.at(j).name<<"..."<<stateList.at(j).elegibleVoters<<" eligible voters"<<endl;
                }
            }
        }
        cout<< endl;
        temp.clear();
    }


}


// checks if the state has been gerrymandered
void gerrymandered(ourvector<state> &stateList,int index,string &gerry,float &totalDemWasted,float &totalRepWasted,int &elligible,float &efficiency){

    float totalVotes=0;

    // looping though all the states
    for (int i = 0; i < stateList.at(index).districts.size(); ++i) {
        float districtDemWasted=0;
        float districtRepWasted=0;
        int democrats = stateList.at(index).districts.at(i).demVotes;
        int republicans = stateList.at(index).districts.at(i).repVotes;
        double districtTotal = democrats+republicans; // total votes in a district
        double overHalf = floor(districtTotal/2)+1; // overhalf for the district

        // get the wasted votes
        if (democrats>republicans){
            districtDemWasted = democrats - overHalf;
            districtRepWasted = republicans;
        } else{
            districtDemWasted = democrats;
            districtRepWasted = republicans - overHalf;

        }
        // total wasted votes for both parties
        totalDemWasted += districtDemWasted;
        totalRepWasted += districtRepWasted;
        totalVotes += districtTotal;

    }

    elligible = stateList.at(index).elegibleVoters;

    efficiency =  (abs(totalDemWasted - totalRepWasted)/totalVotes)*100;

    gerry = (efficiency>=7 && (stateList.at(index).districts.size()>=3)) ? "Yes":"No";

}

// draws the plot for each district
void plot(ourvector<state> &stateList,int &index){
    float demPercent = 0.0;

    // loop through the district vector for the chosen state
    for (int i = 0; i < stateList.at(index).districts.size(); ++i) {
        double total = (stateList.at(index).districts.at(i).demVotes + stateList.at(index).districts.at(i).repVotes);

        // calculate the percentage
        demPercent = floor((double(stateList.at(index).districts.at(i).demVotes)/total)*100);
        cout<< "District: "<< stateList.at(index).districts.at(i).districtId<<endl;
        //draw the number of D's and R's
        cout<< setw(demPercent)<<setfill('D')<<"";
        cout<< setw(100-demPercent) <<setfill('R')<<""<<endl;
        cout<<endl;
    }

}

// helper function to split the command into a main and subcommand
void splitCommand(string &command,string &subCommand,string &mainCommand) {
    unsigned int space = command.find(" ");
    if (space != string::npos) {

        subCommand = command.substr(space + 1);
    }
    mainCommand = command.substr(0, space);
}

// prints the stats for a state
void printStats(string &gerry,float &totalDemWasted,float &totalRepWasted,float &efficiency,int &elligible){
    cout<< "GerryMandered: "<<gerry<<endl;
    if (gerry == "Yes"){
        string against = (totalDemWasted>totalRepWasted) ? "Democrats":"Republicans";
        cout<<"GerryMandered Against: "<< against<<endl;
        cout<<"Efficiency Factor: "<<efficiency<<"%"<<endl;
    }

    cout<<"Wasted Democratic votes: " << int(totalDemWasted)<<endl;
    cout<<"Wasted Republican votes: "<< int(totalRepWasted)<<endl;
    cout<< "Eligible voters: " <<elligible<<endl;
    cout<<endl;

}


int main(){
   ourvector<state> stateList; // main container for all the data
   string command;
   string mainCommand;
   string subCommand;
   string stateName = "N/A";
   int indexOfState;
   bool voterFileLoaded = false;
   bool districtFileLoaded = false;
   cout<< "Welcome to the Gerrymandering App!"<<endl;
   cout<<endl;
   cout<< "Data Loaded? No"<<endl;
   cout<<"State: N/A"<<endl;
   cout<<endl;
   cout<<"Enter command: ";
   // get input
   getline(cin,command);
   cout<<"-----------------------------"<<endl;
   cout<<endl;
   splitCommand(command,subCommand,mainCommand);
   while(mainCommand !="exit" ){
       // check the main command
       if(lower(mainCommand) == "load" ){
           // check if both files are already loaded
           if (voterFileLoaded && districtFileLoaded){
               cout<<"Already read data in, exit and start over."<<endl;
               cout<<endl;
           }else{
               string distFile;
               string voterFile;
               // get the name of both the files in the varibles
               splitCommand(subCommand,voterFile,distFile);
               // load the district file
               loadDistFile(distFile,stateList,districtFileLoaded);
               // check if first file was loaded
               if(districtFileLoaded){
                   // load the voter file
                   loadVoterFile(voterFile,stateList,voterFileLoaded);
               }

           }

       }
       // search command
       if(lower(mainCommand)=="search") {
           // check if the data has been loaded
           if(!voterFileLoaded && !districtFileLoaded){
               cout<<"No data loaded, please load data first."<<endl;
               cout<<endl;
           }else{
               // find the state and get the index of the state
               for (int i = 0; i < stateList.size(); ++i) {
                   if (lower(subCommand) == lower(stateList.at(i).name)) {
                       stateName = stateList.at(i).name;
                       indexOfState = i;
                   }
               }
               // check if state does not exist
               if (lower(stateName) != lower(subCommand)){
                   cout<<"State does not exist, search again."<<endl;
                   cout<<endl;
               }

           }

       }
       // stats command
       if (lower(mainCommand)=="stats"){
           // check if the voter file is loaded
           if(voterFileLoaded){
               if(stateName != "N/A"){
                   float totalRepWasted=0;
                   float totalDemWasted=0;
                   int elligible=0;
                   string gerry= "";
                   float efficiency=0;
                   // set all the stats
                   gerrymandered(stateList,indexOfState,gerry,totalDemWasted,totalRepWasted,elligible,efficiency);
                   // print all the stats
                   printStats(gerry,totalDemWasted,totalRepWasted,efficiency,elligible);

               }else{
                   cout<<"No state indicated, please search for state first."<<endl;
               }

           }else{
               cout << "No data loaded, please load data first." << endl;
           }

       }
       // plot command
       if (lower(mainCommand)=="plot"){
           // check if voter file is loaded
           if(!voterFileLoaded) {
               cout << "No data loaded, please load data first." << endl;
               cout << endl;
           }else{
               // check if a state is searched
               if(stateName=="N/A"){
                   cout<<"No state searched"<<endl;
                   cout<<endl;
               }else{
                   plot(stateList,indexOfState);
               }
           }
       }

       // check if both files are loaded
       if(voterFileLoaded && districtFileLoaded){
           cout<< "Data Loaded? Yes" <<endl;
       }else{
           cout<< "Data Loaded? No" <<endl;
       }

       cout<<"State: "<<stateName<<endl;
       cout<<endl;
       cout<<"Enter command: ";
       // get input again
       getline(cin,command);
       cout<<"-----------------------------"<<endl;
       cout<<endl;
       splitCommand(command,subCommand,mainCommand);

   }

    return 0;
}

