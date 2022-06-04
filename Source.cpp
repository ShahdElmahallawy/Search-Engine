/*
Name:Shahd Khaled Elmahallawy
ID: 900194441
Final Project
*/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
using namespace std;


unordered_map<string, vector<string>> web_graph;// adjacency list to express the graph connecting between all websites.
unordered_map<string, vector<string>> keyword;// the key is URL and the mapped value is the keyword of this URL
unordered_map<string, vector<int>> number_of_impressions;// the key is URL and the mapped value is the number of impressions
unordered_map<string, double> page_rank;// the key is the URL of the weppage and the mapped value is the rank
map<string, int> visited;// To mark visited websites
map<double, string, greater<double>> target_sites;// to store the websites that includes the keywords entered by the user in an order manner
unordered_map<string, double>::iterator it1;
unordered_map<string, vector<int>>::iterator it2;

// functions prototypes to avoid the problem of order
void readFiles(string fname);
void updateFiles();
void rank_webpage();
void CTR();
void split_string(string query, vector<string>& words);
void quotes_case(string query);
void AND_case(string query);
void OR_case(string query);
void search();
void get_keywords();
void results();
void view_webpage_window();
void search_results_window();
void start_window();
bool isNumber(string s);

int main()
{
	readFiles("Webpage.csv");
	readFiles("Keyword.csv");
	readFiles("Number_of_impressions.csv");
	rank_webpage();
	start_window();
	return 0;
}

// A functio o read the files only by sending its name
void readFiles(string fname)
{
	fstream fin;
	fin.open(fname, ios::in | ios::binary);
	string line, word, fword;
	if (fin.is_open())
	{
		while (getline(fin, line))
		{
			int count = 0;
			stringstream s(line);
			getline(s, fword, ',');
			while (getline(s, word, ','))
			{

				if (fname == "Webpage.csv")
				{
					web_graph[fword].push_back(word);
				}
				else if (fname == "Keyword.csv")
				{
					keyword[fword].push_back(word);
				}

				else if (fname == "Number_of_impressions.csv")
				{
					int x;
					x = stoi(word);
					count++;
					if (count % 2 != 0)
					{
						number_of_impressions[fword].push_back(x);
					}
					else
					{
						number_of_impressions[fword].push_back(x);
					}
				}
			}
		}
		fin.close();
	}
	else
	{
		cout << "Couldn't open the file" << fname << endl;
	}
}
// to update CTR for each webpage
void updateFiles() // col[0]=impressions,col[1]=clicks
{
	fstream fout;
	fout.open("Number_of_impressions.csv", ios::out);
	if (fout.fail())
	{
		cout << "Couldn't open the file" << endl;
		exit(0);
	}

	for (auto& element : number_of_impressions)
	{
		//fout << element.first << ',' << element.second[0] << '\n';
		fout << element.first << ',' << element.second[0] << ',' << element.second[1] << '\n';
	}
	fout.close();
}

void rank_webpage()
{
	// First iteration
	for (auto const& pair : keyword)
	{
		page_rank[pair.first] = 1.0 / (keyword.size()); // pair.first has the website name
	}

	// rest of the iterations
	double stop = 0.0001;
	unordered_map<string, double> current;
	current = page_rank;
	double temp_rank;

	for (auto i : keyword)
		visited[i.first] = 0;

	int iterations = 0;
	const int MaxIterations = 5000;
	while (iterations++ < MaxIterations) // Loop untill the diiference of between the iterations= Stop
	{
		int count = 1;
		for (auto const& i : keyword) // parent
		{
			for (auto const& j : web_graph[i.first]) // childern
			{
				// update pr each itertaion as following
				// pr(p) = sum (previous pr(parent)/number of childern)
				temp_rank = page_rank[i.first] / (web_graph[i.first].size());

				if (!visited[j])
				{
					current[j] = temp_rank;
					visited[j] = 1;
				}
				else
				{
					current[j] += temp_rank;
				}
			}
		}
		for (auto i : keyword)
		{
			if (abs(page_rank[i.first] - current[i.first]) < stop)
				count++;
		}
		page_rank = current;
		if (count == keyword.size() - 1)
			break;
	}
	double x, great_prev = 10000.0;
	string temp;
	for (auto i : keyword)
		visited[i.first] = 0;
	for (int i = keyword.size() - 1; i >= 1; i--)
	{
		x = 0.0;
		for (auto j : keyword)
			if (page_rank[j.first] >= x && page_rank[j.first] < great_prev && !visited[j.first])
			{
				x = page_rank[j.first];
				temp = j.first;
			}
		visited[temp] = 1;
		page_rank[temp] = i;
		great_prev = x;
	}
}
void CTR()
{
	string temp;
	int temp_pagerank = 1, temp_impressions = 1, temp_clicks = 1;
	vector<double> scores;
	vector<string> sites;
	double score;
	for (auto i : target_sites)
	{
		it1 = page_rank.find(i.second);
		temp_pagerank = it1->second;

		temp_impressions = number_of_impressions[i.second][0];
		temp_clicks = number_of_impressions[i.second][1];

		double ctr = ((1.0 * temp_clicks) / temp_impressions) * 100;
		score = 0.4 * temp_pagerank + ((1 - ((0.1 * temp_impressions) / (1 + 0.1 * temp_impressions))) * temp_pagerank + ((0.1 * temp_impressions) / (1 + 0.1 * temp_impressions)) * ctr) * 0.6;
		scores.push_back(score);
		string tempSite = i.second;
		sites.push_back(tempSite);
	}
	target_sites.clear();
	for (int i = 0; i < scores.size(); i++)
		target_sites[scores[i]] = sites[i];
}

// Function to split query into words and push them back in a string vector
void split_string(string query, vector<string>& words)
{
	string temp = "";
	for (int i = 0; i < query.length(); ++i)
	{

		if (query[i] == ' ')
		{
			words.push_back(temp);
			temp = "";
		}
		else
		{
			temp.push_back(query[i]);
		}
	}
	words.push_back(temp);
}
void quotes_case(string query)
{
	int count = 0;
	double c = 100000.0;
	for (auto const& pair : keyword)
	{
		int count = 0;
		for (auto& key : pair.second)
		{
			if (query == key)
				count++;
		}
		if (count == 1)
		{
			target_sites[c] = pair.first;
			c++;
		}
	}
}
void AND_case(string query)
{
	double c = 100000.0;
	vector<string> words;
	split_string(query, words);
	int count = 0;
	for (auto const& pair : keyword)
	{
		int count = 0;
		for (auto& word : words)
		{
			for (auto& key : pair.second)
			{
				if (word == key)
					count++;
			}
		}
		if (count == words.size()) // words - "AND"
		{
			target_sites[c] = pair.first;
			c++;
		}
	}
}
void OR_case(string query)
{
	double c = 100000.0;
	vector<string> words;
	split_string(query, words);
	for (auto const& pair : keyword)
	{
		int count = 0;
		for (auto& word : words)
		{
			for (auto& key : pair.second)
			{
				if (word == key)
					count++;
			}
		}
		if (count >= 1)
		{
			target_sites[c] = pair.first;
			c++;
		}
	}
}

void search()
{
	target_sites.clear();

	int type = 0;
	bool flag = false; // it is true special case("", AND, OR), otherwise
	cout << "Please, enter a keyword:" << endl;
	string query;
	getline(cin >> ws, query); // eat whitespace
	string temp;
	if (query[0] == '"')
	{
		flag = true;
		// to remove qoutes
		for (int i = 1; i < query.size() - 1; i++)
		{
			temp.push_back(query[i]);
		}
		quotes_case(temp);
	}
	else
	{
		for (int i = 0; i < query.size(); i++)
		{
			// to rempve AND
			if (query[i] == 'A' && query[i + 1] == 'N' && query[i + 2] == 'D')
			{
				flag = true;
				i = i + 4;
				temp.push_back(query[i]);
				type = 1;
			}
			// To remove OR
			else if (query[i] == 'O' && query[i + 1] == 'R')
			{
				flag = true;
				i = i + 3;
				temp.push_back(query[i]);
				type = 2;
			}
			// push the words as it is if it is not one of the previos cases
			else
			{
				temp.push_back(query[i]);
			}
		}
	}
	if (type == 1)
	{
		AND_case(temp);
	}
	else if (type == 2)
	{
		OR_case(temp);
	}
	else if (flag == false)
	{
		OR_case(temp);
	}
}
// To print the final results we got
void results()
{
	int c = 1;
	cout << "Search results:" << endl;
	for (auto i : target_sites)
	{
		// updateFiles(i.second, 0);
		number_of_impressions[i.second][0]++;
		cout << c << ".   " << i.second << endl;
		c++;
	}
}
void view_webpage_window()
{
	cout << "Please, enter the name of webpage without spaces:" << endl;
	string url;
	cin >> url;
	cout << "-----------------------------------" << endl;
	bool found = 0;
	for (auto i : target_sites)
	{
		if (url == i.second)
		{
			found = 1;
			number_of_impressions[url][1]++;
			// updateFiles(url, 1);
			cout << "You're now veiwing " << url << endl;
		}
	}
	if (!found)
	{
		cout << "This website is not found!" << endl;
	}
	cout << "Would you like to" << endl;
	cout << "1. Back to search results" << endl;
	cout << "2.New Search" << endl;
	cout << "3.Exit" << endl;
	string choice;
	cin >> choice;
	cout << "-----------------------------------" << endl;
	while (isNumber(choice) == false)
	{
		cout << "Please, enter a valid choice:" << endl;
		cin >> choice;
	}
	while (stoi(choice) != 1 && stoi(choice) != 2 && stoi(choice) != 3)
	{
		cout << "Please, enter a valid choice: " << endl;
		cin >> choice;
		cout << "-----------------------------------" << endl;
	}
	if (stoi(choice) == 1) // Back to search result
	{
		search_results_window();
	}
	else if (stoi(choice) == 2) // New search
	{
		get_keywords();
	}
	else if (stoi(choice) == 3) // Exit
	{
		updateFiles();
		exit(0);
	}
}
void search_results_window()
{
	// a numbered results list(sorted by webpage score)
	results();

	cout << "Would you like to" << endl;
	cout << "1. choose a webpage to open" << endl;
	cout << "2. New search" << endl;
	cout << "3. Exit" << endl;
	cout << "Type in your choice: " << endl;
	string choice;
	cin >> choice;
	cout << "-----------------------------------" << endl;
	while (isNumber(choice) == false)
	{
		cout << "Please, enter a valid choice:" << endl;
		cin >> choice;
	}
	while (stoi(choice) != 1 && stoi(choice) != 2 && stoi(choice) != 3)
	{
		cout << "Please, enter a valid choice: " << endl;
		cin >> choice;
	}
	if (stoi(choice) == 1) // choose a webpage to open
	{
		view_webpage_window();
	}
	else if (stoi(choice) == 2) // New Search
	{
		get_keywords();
	}
	else if (stoi(choice) == 3) // Exit
	{
		updateFiles();
		exit(0);
	}
}
void get_keywords()
{
	search();
	while (target_sites.empty())
	{
		cout << "Keyword is not found!" << endl;
		search();
	}
	CTR();
	cout << "-----------------------------------" << endl;
	search_results_window();
}
void start_window()
{
	cout << "Welcome!" << endl;
	cout << "What would you like to do?" << endl;
	cout << "1. New Search" << endl;
	cout << "2. Exit" << endl;
	cout << "Type in your choice: " << endl;
	string choice;
	cin >> choice;
	cout << "-----------------------------------" << endl;
	while (isNumber(choice) == false)
	{
		cout << "Please, enter a valid choice:" << endl;
		cin >> choice;
	}
	while (stoi(choice) != 2 && stoi(choice) != 1)
	{
		cout << "Please, enter a valid choice:" << endl;
		cin >> choice;
	}
	if (stoi(choice) == 1) // new search
	{
		get_keywords(); // get keywords first and get the websites that include them
	}
	else if (stoi(choice) == 2) // Exit
	{
		updateFiles();
		exit(0);
	}
}
bool isNumber(string s)
{
	for (int i = 0; i < s.size(); i++)
		if (isdigit(s[i]) == false)
			return false;
	return true;
}