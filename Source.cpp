#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <queue>
#include <stdio.h>
#include "sha256.h"
using namespace std;

struct line{//線結構
	string num0;
	string num1;
};
struct gate{//Gate結構
	string operation;
	int x;
	int y;
	int out;
};
struct gate_table{
	string name;
	string table[4];
};

string ran(){//Key生產器，0 1是用來辨別是否需要特殊處理
	string re = "";
	int ran_num = rand();
	ran_num = ran_num % 256;//亂數0~255
	bitset<8> b(ran_num);//換回8-bit
	re = b.template to_string<char, char_traits<char>, allocator<char> >();
	return re;
}
string encoding(string a, string b){//底層加密
	string out="";
	for(int i=0;i!=8;i++){
		if(a[i]=='0'&&b[i]=='0'){
			out = out + '0';
		}
		else if(a[i] == '0'&&b[i] == '1'){
			out = out + '1';
		}
		else if(a[i] == '1'&&b[i] == '0'){
			out = out + '1';
		}
		else if(a[i] == '1'&&b[i] == '1') {
			out = out + '0';
		}
	}
	return out;
}

int main(){
	srand(time(NULL));
	int line_num = 0, gate_num = 0, in_line_num = 0, out_line_num = 0, count = 0;//線的總數，Gate的總數，輸入線的數量，輸出線的數量
	string reg_st, reg_cut;//暫存一整行，暫存經分割的一個字串

	fstream in;//1個Input檔和4個Output檔
	in.open("ciruit.txt", ios::in);

	line *save_line = 0;//把每一條線的兩種訊號存起來
	queue<gate> save_gate;//把每一個gate存起來
	
	line *output_line = 0;
	queue<gate> output_gate;
	
	while (getline(in, reg_st))
	{
		if (reg_st[0] != '/'){//註解跳過，其他執行
			if (count == 0)
			{//處理第一行
				reg_cut = "";
				for (int i = 0; i <= reg_st.size(); i++)
				{//分割
					if (reg_st[i] != ' '&&i < reg_st.size())
					{
						reg_cut = reg_cut + reg_st[i];
					}
					else
					{
						count++;
						int reg_num = 0;
						for (int j = reg_cut.size(); j != 0; j--)//換成int
							reg_num = reg_num + (reg_cut[j - 1] - '0')*pow(10, reg_cut.size() - j);
						switch (count)
						{//存到對應的空間
						case 1:
							line_num = reg_num;
							save_line = new line[line_num + 1];
							output_line = new line[line_num + 1];
							break;
						case 2:
							gate_num = reg_num;
							break;
						case 3:
							in_line_num = reg_num;
							break;
						case 4:
							out_line_num = reg_num;
							break;
						}
						reg_cut = "";//還原預設值
					}
				}
				count = 0;
			}
			else{
				if (reg_st[0] != '/')
				{
					int step = 0;
					gate reg_gate;//把需要記起來的gate的資料記起來，最後push進queue中
					reg_cut = "";//預設值
					for (int i = 0; i <= reg_st.size(); i++)
					{
						if (reg_st[i] != ' '&&i < reg_st.size())
						{//分割
							reg_cut = reg_cut + reg_st[i];
						}
						else
						{
							step++;
							int reg_num = 0;
							if (step != 3)
							{//除了step=3外，其他都要換成int
								for (int j = reg_cut.size(); j != 0; j--)//換成int
									reg_num = reg_num + (reg_cut[j - 1] - '0')*pow(10, reg_cut.size() - j);
							}
							
							switch (step)
							{
							case 1:
								reg_gate.x = reg_num;
								if (save_line[reg_num].num0.size() != 8)
								{
									save_line[reg_num].num0 = ran();
									save_line[reg_num].num1 = ran();
								}
								break;
							case 2:
								reg_gate.y = reg_num;
								if (save_line[reg_num].num0.size() != 8)
								{
									save_line[reg_num].num0 = ran();
									save_line[reg_num].num1 = ran();
								}
								break;
							case 3://把operation換成代碼，並輸出到gcircuit.txt
								reg_gate.operation = reg_cut;
								break;
							case 4:
								reg_gate.out = reg_num;
								save_gate.push(reg_gate);
								if (save_line[reg_num].num0.size() != 8)
								{
									save_line[reg_num].num0 = ran();
									save_line[reg_num].num1 = ran();
								}
								string reg = " ";
								reg[0]=count+'0';
								reg_gate.operation = "gate"+reg;
								output_gate.push(reg_gate);
								break;
							}
							reg_cut = "";//還原預設值
						}
					}
				}
			}
			count++;//一行完結，準備處理下一行
		}
	}
	for(int i=1;i<=in_line_num;i++){
		output_line[i].num0=save_line[i].num0;
		output_line[i].num1=save_line[i].num1;
	}
	for(int i=in_line_num+1;i<=line_num-out_line_num;i++){
		output_line[i].num0=sha256(save_line[i].num0);
		output_line[i].num1=sha256(save_line[i].num1);
	}
	for(int i=line_num-out_line_num+1;i<=line_num;i++){
		output_line[i].num0=save_line[i].num0;
		output_line[i].num1=save_line[i].num1;
	}
	
	queue<gate_table> g_table;
	gate_table save;
	int count1=1;
	while (!save_gate.empty())
	{//進行加密，然後把順序打亂，再把各個gate的對應表存起來
		save.name = "gate";
		string reg = " ";
		reg[0]=count1+'0';
		save.name = "gate"+reg;
		string truth_table;//把truth table記起來
		if (save_gate.front().operation == "AND")
			truth_table = "0001";
		else if (save_gate.front().operation == "NAND")
			truth_table = "1110";
		else if (save_gate.front().operation == "OR")
			truth_table = "0111";
		int count_t[4] = { 0, 0, 0, 0 },count0 = 0;//記錄已加密完成的
		while (count_t[0] != 1 || count_t[1] != 1 || count_t[2] != 1 || count_t[3] != 1)
		{//當4個都加密完才停止
			int i = rand() % 4;//亂數1~4
			if (count_t[i] != 1)
			{
				if (i == 0 && truth_table[0] == '0'){
					save.table[count0]=encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num0));
				}
				else if (i == 0 && truth_table[0] == '1') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num1));
				}
				else if (i == 1 && truth_table[1] == '0') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num0));
				}
				else if (i == 1 && truth_table[1] == '1') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num1));
				}
				else if (i == 2 && truth_table[2] == '0') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num0));
				}
				else if (i == 2 && truth_table[2] == '1') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num1));
				}
				else if (i == 3 && truth_table[3] == '0') {
					save.table[count0]=encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num0));
				}
				else if (i == 3 && truth_table[3] == '1') {
					save.table[count0] = encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num1));
				}
				count_t[i] = 1;
				count0++;
			}
		}
		count1++;
		g_table.push(save);
		save_gate.pop();
	}
	
	int *inp;
	inp = new int[in_line_num];
	cout << "Please input " << in_line_num << " binary number:" << endl;
	for (int i = 0; i != in_line_num; i++)
		cin >> inp[i];

	//cout << endl;

	//cout << line_num << " " << gate_num << " " << in_line_num << " " << out_line_num << endl;

	for (int i = 0; i != in_line_num; i++) {
		if (inp[i] == 0) {
			output_line[i + 1].num1 = output_line[i + 1].num0;
		}
		else {
			output_line[i + 1].num0 = output_line[i + 1].num1; 
		}
	}

	/*while (!output_gate.empty()) {
		cout << output_gate.front().x << " " << output_gate.front().y << " " << output_gate.front().operation << " " << output_gate.front().out << endl;
		output_gate.pop();
	}

	cout << endl;

	while(!g_table.empty()){
		cout << g_table.front().name << " "
		<< g_table.front().table[0] << " "
		<< g_table.front().table[1] << " "
		<< g_table.front().table[2] << " "
		<< g_table.front().table[3] << " "
		<< endl;
		g_table.pop();
	}

	cout << endl;

	for(int i=1;i<=10;i++)
		cout << output_line[i].num0 << " " << output_line[i].num1 << endl;*/

	string *de_reg;
	de_reg = new string[line_num + 1];

	for (int i = 1; i <= in_line_num; i++) {
		de_reg[i] = output_line[i].num0;
		cout << de_reg[i] << " " << endl;
	}

	int de_count = in_line_num + 1;

	while(g_table.size()!=out_line_num){
		for(int i=0;i!=4;i++){
			string check = sha256(encoding(de_reg[output_gate.front().y], encoding(de_reg[output_gate.front().x],g_table.front().table[i])));
			if (check == output_line[de_count].num0 || check == output_line[de_count].num1) {
				de_reg[de_count] = encoding(de_reg[output_gate.front().y], encoding(de_reg[output_gate.front().x], g_table.front().table[i]));
				break;
			}
			else if(i==3){
				cout << "error" << endl;
			}
		}
		de_count++;
		output_gate.pop();
		g_table.pop();
	}
	while (!g_table.empty()) {
		for (int i = 0; i != 4; i++) {
			string check = encoding(de_reg[output_gate.front().y], encoding(de_reg[output_gate.front().x], g_table.front().table[i]));
			if (check == output_line[de_count].num0 || check == output_line[de_count].num1) {
				cout << de_reg[output_gate.front().x] << " " << de_reg[output_gate.front().y] << endl;
				de_reg[de_count] = check;
				break;
			}
			else if(i==3){
				cout << "error" << endl;
			}
		}
		output_gate.pop();
		g_table.pop();
	}
	cout << "Output: ";
	for (int i=line_num - out_line_num+1;i<=line_num;i++){
		if (de_reg[i] == output_line[i].num0)
			cout << 0;
		else if (de_reg[i] == output_line[i].num1)
			cout << 1;
	}
	cout << endl;

	system("pause");
	return 0;
}