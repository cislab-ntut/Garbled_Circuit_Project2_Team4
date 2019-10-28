#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <queue>
using namespace std;

struct line{//�u���c
	string num0;
	string num1;
};
struct gate{//Gate���c
	string operation;
	int x;
	int y;
	int out;
};

string ran(int signal){//Key�Ͳ����A0 1�O�Ψӿ�O�O�_�ݭn�S��B�z
	string re="";
	int ran_num=rand();
	ran_num=ran_num%256;//�ü�0~255
	bitset<8> b(ran_num);//���^8-bit
	re =  b.template to_string<char,char_traits<char>, allocator<char> >( );
	return re;
}
string encoding(string a,string b){//���h�[�K
	return b;
}

int main(){
	srand(time(NULL));
	int line_num,gate_num,in_line_num,out_line_num,count=0;//�u���`�ơAGate���`�ơA��J�u���ƶq�A��X�u���ƶq
	string reg_st,reg_cut;//�Ȧs�@���A�Ȧs�g���Ϊ��@�Ӧr��
	
	fstream in,gcircuit,input_key,gate_table,output_table;//1��Input�ɩM4��Output��
	in.open("ciruit.txt",ios::in);
	gcircuit.open("gcircuit.txt",ios::out);
	input_key.open("input_key.txt",ios::out);
	gate_table.open("gate_table.txt",ios::out);
	output_table.open("output_table.txt",ios::out);
	
	line *save_line;//��C�@���u����ذT���s�_��
	queue<gate> save_gate;//��C�@��gate�s�_��
	while(getline(in,reg_st)){
		if (reg_st[0]!='/'){//���Ѹ��L�A��L����
			if(count ==0){//�B�z�Ĥ@��
				reg_cut="";
				for(int i=0;i<=reg_st.size();i++){//����
					if(reg_st[i]!=' '&&i<reg_st.size()){
						reg_cut=reg_cut+reg_st[i];
					}
					else{
						count++;
						int reg_num=0;
						for(int j=reg_cut.size();j!=0;j--)//����int
							reg_num=reg_num+(reg_cut[j-1]-'0')*pow(10,reg_cut.size()-j);
						switch (count){//�s��������Ŷ�
							case 1:
								line_num=reg_num;
								save_line = new line[line_num+1];
								break;
							case 2:
								gate_num=reg_num;
								break;
							case 3:
								in_line_num=reg_num;
								break;
							case 4:
								out_line_num=reg_num;
								break;
						}
						reg_cut="";//�٭�w�]��
					}
				}
				count=0;
			}
			else{
				if (reg_st[0]!='/'){
					int step=0;
					gate reg_gate;//��ݭn�O�_�Ӫ�gate����ưO�_�ӡA�̫�push�iqueue��
					reg_cut="";//�w�]��
					for(int i=0;i<=reg_st.size();i++){
						if(reg_st[i]!=' '&&i<reg_st.size()){//����
							reg_cut=reg_cut+reg_st[i];
						}
						else{
							step++;
							int reg_num=0;
							if(step!=3){//���Fstep=3�~�A��L���n����int
								for(int j=reg_cut.size();j!=0;j--)//����int
									reg_num=reg_num+(reg_cut[j-1]-'0')*pow(10,reg_cut.size()-j);
							}
							switch(step){
								case 1:
									gcircuit << reg_num << " ";
									reg_gate.x=reg_num;
									if(save_line[reg_num].num0.size()!=8){
										if(reg_num<=in_line_num){
											save_line[reg_num].num0=ran(0);
											save_line[reg_num].num1=ran(0);
										}
										else{
											save_line[reg_num].num0=ran(1);
											save_line[reg_num].num1=ran(1);
										}
									}
									break;
								case 2:
									gcircuit << reg_num << " ";
									reg_gate.y=reg_num;
									if(save_line[reg_num].num0.size()!=8){
										if(reg_num<=in_line_num){
											save_line[reg_num].num0=ran(0);
											save_line[reg_num].num1=ran(0);
										}
										else{
											save_line[reg_num].num0=ran(1);
											save_line[reg_num].num1=ran(1);
										}
									}
									break;
								case 3://��operation�����N�X�A�ÿ�X��gcircuit.txt
									gcircuit << "gate" << count << " ";
									reg_gate.operation=reg_cut;
									break;
								case 4:
									gcircuit << reg_num << endl;
									reg_gate.out=reg_num;
									save_gate.push(reg_gate);
									if(save_line[reg_num].num0.size()!=8){
										if(reg_num<=in_line_num){
											save_line[reg_num].num0=ran(0);
											save_line[reg_num].num1=ran(0);
										}
										else{
											save_line[reg_num].num0=ran(1);
											save_line[reg_num].num1=ran(1);
										}
									}
									break;
								step++;
							}
							reg_cut="";//�٭�w�]��
						}
					}
				}
			}
			count++;//�@�槹���A�ǳƳB�z�U�@��
		}
	}
	for(int i=1;i<=in_line_num;i++){//�sinput������
		input_key << save_line[i].num0 << " " << save_line[i].num1 << endl;
	}
	for(int i=line_num-out_line_num+1;i<=line_num;i++){////�soutput������
		output_table << i << ": " << save_line[i].num0 << " " << save_line[i].num1 << endl;
	}
	while(!save_gate.empty()){//�i��[�K�A�M��ⶶ�ǥ��áA�A��U��gate��������s�_��
		gate_table << "gate"  << gate_num-save_gate.size()+1 << ":" << endl;
		string truth_table;//��truth table�O�_��
		if(save_gate.front().operation=="AND")
			truth_table="0001";
		else if(save_gate.front().operation=="NAND")
			truth_table="1110";
		else if(save_gate.front().operation=="OR")
			truth_table="0111";
		int count_t[4]={0,0,0,0};//�O���w�[�K������
		while(count_t[0]!=1||count_t[1]!=1||count_t[2]!=1||count_t[3]!=1){//��4�ӳ��[�K���~����
			int i = rand()%4;//�ü�1~4
			if(count_t[i]!=1){
				if(i==0&&truth_table[0]=='0')
					gate_table << encoding(save_line[save_gate.front().x].num0,encoding(save_line[save_gate.front().y].num0,save_line[save_gate.front().out].num0))<<endl;
				else if(i==0&&truth_table[0]=='1')
					gate_table << encoding(save_line[save_gate.front().x].num0,encoding(save_line[save_gate.front().y].num0,save_line[save_gate.front().out].num1))<<endl;
				else if(i==1&&truth_table[1]=='0')
					gate_table << encoding(save_line[save_gate.front().x].num0,encoding(save_line[save_gate.front().y].num1,save_line[save_gate.front().out].num0))<<endl;
				else if(i==1&&truth_table[1]=='1')
					gate_table << encoding(save_line[save_gate.front().x].num0,encoding(save_line[save_gate.front().y].num1,save_line[save_gate.front().out].num1))<<endl;
				else if(i==2&&truth_table[2]=='0')
					gate_table << encoding(save_line[save_gate.front().x].num1,encoding(save_line[save_gate.front().y].num0,save_line[save_gate.front().out].num0))<<endl;
				else if(i==2&&truth_table[2]=='1')
					gate_table << encoding(save_line[save_gate.front().x].num1,encoding(save_line[save_gate.front().y].num0,save_line[save_gate.front().out].num1))<<endl;
				else if(i==3&&truth_table[3]=='0')
					gate_table << encoding(save_line[save_gate.front().x].num1,encoding(save_line[save_gate.front().y].num1,save_line[save_gate.front().out].num0))<<endl;
				else if(i==3&&truth_table[3]=='1')
					gate_table << encoding(save_line[save_gate.front().x].num1,encoding(save_line[save_gate.front().y].num1,save_line[save_gate.front().out].num1))<<endl;
				count_t[i]=1;
			}
		}
		save_gate.pop();
	}
	return 0;
}
