#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <queue>
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

string ran(int signal){//Key生產器，0 1是用來辨別是否需要特殊處理
	string re = "";
	int ran_num = rand();
	ran_num = ran_num % 256;//亂數0~255
	bitset<8> b(ran_num);//換回8-bit
	re = b.template to_string<char, char_traits<char>, allocator<char> >();
	string output1 = sha256(re);
		cout << "sha256('" << re << "'):" << output1 << endl;
	return output1;
}
string encoding(string a, string b){//底層加密
	return b;
}

int main(){
	srand(time(NULL));
	int line_num = 0, gate_num = 0, in_line_num = 0, out_line_num = 0, count = 0;//線的總數，Gate的總數，輸入線的數量，輸出線的數量
	string reg_st, reg_cut;//暫存一整行，暫存經分割的一個字串

	fstream in, gcircuit, input_key, gate_table, output_table;//1個Input檔和4個Output檔
	in.open("ciruit.txt", ios::in);
	gcircuit.open("gcircuit.txt", ios::out);
	input_key.open("input_key.txt", ios::out);
	gate_table.open("gate_table.txt", ios::out);
	output_table.open("output_table.txt", ios::out);

	line *save_line = 0;//把每一條線的兩種訊號存起來
	queue<gate> save_gate;//把每一個gate存起來
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
								gcircuit << reg_num << " ";
								reg_gate.x = reg_num;
								if (save_line[reg_num].num0.size() != 8)
								{
									if (reg_num <= in_line_num)
									{
										save_line[reg_num].num0 = ran(0);
										save_line[reg_num].num1 = ran(0);
									}
									else
									{
										save_line[reg_num].num0 = ran(1);
										save_line[reg_num].num1 = ran(1);
									}
								}
								break;
							case 2:
								gcircuit << reg_num << " ";
								reg_gate.y = reg_num;
								if (save_line[reg_num].num0.size() != 8)
								{
									if (reg_num <= in_line_num)
									{
										save_line[reg_num].num0 = ran(0);
										save_line[reg_num].num1 = ran(0);
									}
									else
									{
										save_line[reg_num].num0 = ran(1);
										save_line[reg_num].num1 = ran(1);
									}
								}
								break;
							case 3://把operation換成代碼，並輸出到gcircuit.txt
								gcircuit << "gate" << count << " ";
								reg_gate.operation = reg_cut;
								break;
							case 4:
								gcircuit << reg_num << endl;
								reg_gate.out = reg_num;
								save_gate.push(reg_gate);
								if (save_line[reg_num].num0.size() != 8)
								{
									if (reg_num <= in_line_num)
									{
										save_line[reg_num].num0 = ran(0);
										save_line[reg_num].num1 = ran(0);
									}
									else
									{
										save_line[reg_num].num0 = ran(1);
										save_line[reg_num].num1 = ran(1);
									}
								}
								break;
								step++;
							}
							reg_cut = "";//還原預設值
						}
					}
				}
			}
			count++;//一行完結，準備處理下一行
		}
	}
	for (int i = 1; i <= in_line_num; i++)
	{//存input對應表
		input_key << save_line[i].num0 << " " << save_line[i].num1 << endl;
	}
	for (int i = line_num - out_line_num + 1; i <= line_num; i++)
	{////存output對應表
		output_table << i << ": " << save_line[i].num0 << " " << save_line[i].num1 << endl;
	}
	while (!save_gate.empty())
	{//進行加密，然後把順序打亂，再把各個gate的對應表存起來
		gate_table << "gate" << gate_num - save_gate.size() + 1 << ":" << endl;
		string truth_table;//把truth table記起來
		if (save_gate.front().operation == "AND")
			truth_table = "0001";
		else if (save_gate.front().operation == "NAND")
			truth_table = "1110";
		else if (save_gate.front().operation == "OR")
			truth_table = "0111";
		int count_t[4] = { 0, 0, 0, 0 };//記錄已加密完成的
		while (count_t[0] != 1 || count_t[1] != 1 || count_t[2] != 1 || count_t[3] != 1)
		{//當4個都加密完才停止
			int i = rand() % 4;//亂數1~4
			if (count_t[i] != 1)
			{
				if (i == 0 && truth_table[0] == '0')
					gate_table << encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num0)) << endl;
				else if (i == 0 && truth_table[0] == '1')
					gate_table << encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num1)) << endl;
				else if (i == 1 && truth_table[1] == '0')
					gate_table << encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num0)) << endl;
				else if (i == 1 && truth_table[1] == '1')
					gate_table << encoding(save_line[save_gate.front().x].num0, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num1)) << endl;
				else if (i == 2 && truth_table[2] == '0')
					gate_table << encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num0)) << endl;
				else if (i == 2 && truth_table[2] == '1')
					gate_table << encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num0, save_line[save_gate.front().out].num1)) << endl;
				else if (i == 3 && truth_table[3] == '0')
					gate_table << encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num0)) << endl;
				else if (i == 3 && truth_table[3] == '1')
					gate_table << encoding(save_line[save_gate.front().x].num1, encoding(save_line[save_gate.front().y].num1, save_line[save_gate.front().out].num1)) << endl;
				count_t[i] = 1;
			}
		}
		save_gate.pop();
	}
	//system("pause");
	return 0;
}
