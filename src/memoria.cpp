//
//	Organização e Arquitetura de Computadores - UnB
//  Turma 1A
//	Período letivo: 2020.2 (3/3/2021)
//
//	Implementação de Funções de acesso à Memória do RISCV RV321 em C
//
//	Aluno: Gabriel Tambara Rabelo
//	Matrícula: 18/0017021
//	Curso: Engenharia de Controle e Automação(Mecatrônica)
//
//	Dados do compiplador e sistema operacional(gcc --version):
//		gcc (Ubuntu 5.4.0-6ubuntu1~16.04.12) 5.4.0 20160609
//	IDE: Sublime text editor
//
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 4096
int32_t mem[MEM_SIZE];

int32_t lw(uint32_t endereco, int32_t kte){

	int32_t valor = endereco + kte;
	if(valor % 4 != 0){
		printf("Erro lw.\nO endereço da memória não é um múltiplo de 4.\n");
		return 0;
	}
	valor /= 4;

	if( valor > MEM_SIZE-1 || valor < 0){
		printf("Erro lw.\nAcesso a memória inexistente(Out of memory).\n");
		return 0;
	}	
	
	return mem[valor];
}

int32_t lb(uint32_t endereco, int32_t kte){

	if( endereco/4 > MEM_SIZE-1 || endereco/4 < 0 || endereco %4 != 0){
		printf("Erro lw.\nAcesso a memória inexistente(Out of memory).\n");
		return 0;
	}

	if(kte > 3 || kte<0){
		printf("Erro lb.\nÍndice de byte da palavra inválido.\n");
		return 0;
	}
	int32_t base = lw(endereco,0);
	uint32_t mascara = 255;
	//usando shift para posicionar a máscara e aplicá-la e depois o mesmo no valor
	mascara <<= 8*kte;
	int32_t saida = base & mascara;
	saida >>= 8*kte;

	if((int8_t)saida < 0){
		mascara = 4294967040;
		saida |= mascara;
	}

	return saida;
}

int32_t lbu(uint32_t endereco, int32_t kte){

	if( endereco/4 > MEM_SIZE-1 || endereco/4 < 0 || endereco %4 != 0){
		printf("Erro lw.\nAcesso a memória inexistente(Out of memory).\n");
		return 0;
	}

	if(kte > 3 || kte<0){
		printf("Erro lbu.\nÍndice de byte da palavra inválido.\n");
		return 0;
	}
	int32_t base = lw(endereco,0);
	uint32_t mascara = 255;
	//usando shift para posicionar a máscara e aplicá-la e depois o mesmo no valor
	mascara <<= 8*kte;
	int32_t saida = base & mascara;
	saida >>= 8*kte;
	saida &= 255;
	
	return saida;
}

void sw(uint32_t endereco, int32_t kte, int32_t dado){

	int32_t valor = endereco + kte;

	if(valor % 4 != 0){
		printf("Erro sw.\nO endereço da memória não é um múltiplo de 4.\n");
		return;
	}
	valor /= 4;

	if( valor > MEM_SIZE || valor < 0){
		printf("Erro sw.\nAcesso a memória inexistente(Out of memory).\n");
		return;
	}	
	mem[valor] = dado;
	return;
}

void sb(uint32_t endereco, int32_t kte, int8_t dado){

	int32_t valor = endereco;

	if(kte > 3 || kte<0){
		printf("Erro sb.\nÍndice de byte da palavra inválido.\n");
		return;
	}
	valor /= 4;

	if( valor > MEM_SIZE || valor < 0){
		printf("Erro sb.\nAcesso a memória inexistente(Out of memory).\n");
		return;
	}	
	uint32_t base = (uint32_t)dado;
	uint32_t mascara_isolante = 0xffffff00;
	uint32_t mascara = 255;
	int32_t temporario = 0;
	//Separando o byte com zeros a esquerda
	base = mascara_isolante | (~base);
	base = ~base;
	base <<= 8*kte;
	mascara <<= 8*kte;
	//Acessando o endereço para substituir o valor corretamente
	temporario = mem[valor];
	temporario &= (~mascara);
	temporario |= base;
	mem[valor] = temporario;
	return;
}

int main(){
	int i;

//	Inicialização da memória
	for(i=0;i < MEM_SIZE;i++){
		mem[i] = 0;
	}

//	Questão 1(Alocando na memória)
	sb(0,0,0x04);
	sb(0,1,0x03);
	sb(0,2,0x02);
	sb(0,3,0x01);
	sb(4,0,0xFF);
	sb(4,1,0xFE);
	sb(4,2,0xFD);
	sb(4,3,0xFC);
	sw(12,0,0xFF);
	sw(16,0,0xFFFF);
	sw(20,0,0xFFFFFFFF);
	sw(24,0,0x80000000);

//	Questão 2(Ler da memória em hexa)
	printf("Questão 2, ler em hexa:\n");
	int p=0;
	while(p<=6){
		printf("%c:%08x\n",97+p,mem[p]);
		p++;
	}
	printf("\n");

//	Questão 3(Ler em hexa)
	printf("Questão 3, ler em hexa:\n");
	printf("a:\n%08x\n%08x\n%08x\n%08x\n",lb(4,0),lb(4,1),lb(4,2),lb(4,3));
	printf("b:\n%08x\n%08x\n%08x\n%08x\n",lbu(4,0),lbu(4,1),lbu(4,2),lbu(4,3));
	printf("c:\n%08x\n%08x\n%08x\n\n\n",lw(12,0),lw(16,0),lw(20,0));

//	Testes alternativos deliberados pelo desenvolvedor:
//	
//	Testes de funções dentro de outras funções:
	printf("Testes com uso conjunto de funções\n");
	sw(860,0,-2555253);
	printf("1) sw(860,0,-2555253):\n %08x\n",lw(860,0));
	sb(860,1,0x0);
	printf("2) sb(860,1,0x0):\n %08x\n",lw(860,0));
	sb(860,2,lb(860,1));
	printf("3) sb(860,2,lb(860,1)):\n %08x\n",lw(860,0));
	sw(864,0,lb(860,0));
	printf("4) sw(864,0,lb(860,0)):\n %08x\n",lw(864,0));
	sw(864,0,lbu(860,0));
	printf("5) sw(864,0,lbu(860,0)):\n %08x\n\n",lw(864,0));

//	Testes de movimentos gerais e operações lógicas
	printf("Testes com movimentos gerais e operações lógicas\n");
	sw(884,0,0);
	printf("1) sw(884,0,0)\n %08x\n",lw(884,0));

	sw(884,0,~lw(884,0));
	printf("2) sw(884,0,~lw(884,0))\n %08x\n",lw(884,0));

	sb(884,0,~lb(884,0));
	printf("3)sb(884,0,~lb(884,0))\n %08x\n",lw(884,0));

	sb(884,2,~lb(884,2));
	printf("4) sb(884,2,~lb(884,2))\n %08x\n",lw(884,0));

	sw(884,0,~lw(884,0));
	printf("5) sw(884,0,~lw(884,0))\n %08x\n",lw(884,0));

	sw(884,0,lw(884,0)|~lw(884,0));
	printf("6) sw(884,0,lw(884,0)|~lw(884,0))\n %08x\n",lw(884,0));

	sw(884,0,lw(884,0)^0xff00ff00);
	printf("7) sw(884,0,lw(884,0)^0xff00ff00)\n %08x\n\n",lw(884,0));

//	Testes de representação numérica
	printf("Testes de representação numérica\n");
	sw(0,0,-0x2f3e4d5c);
	printf("1) sw(0,0,-0x2f3e4d5c):\n   %d = 0x%08x\n",lw(0,0),lw(0,0));
	sw(0,0,0x2f3e4d5c);
	printf("2) sw(0,0,0x2f3e4d5c):\n   %d = 0x%08x\n",lw(0,0),lw(0,0));
	sw(0,0,0xfe);
	printf("3) sw(0,0,0xfe):\n   %d = 0x%08x\n",lw(0,0),lw(0,0));
	sw(0,0,0xffffffff);
	printf("3) sw(0,0,0xffffffff):\n   %d = 0x%08x\n\n",lw(0,0),lw(0,0));

//	Inicialização da memória
	for(i=0;i < MEM_SIZE;i++){
		mem[i] = 0;
	}

//	Testes de leitura de bytes extendidos

	printf("Testes de leitura de bytes extendidos(por endereço)\n");
	sw(0,0,0xa403c2d1);
	sw(4,0,0xe8f7a6b5);
	printf("sw(0,0,0xa403c2d1)\nsw(4,0,0xe8f7a6b5)\n");
	int k;
	printf("Palavra 1:\n");
	for(k=0;k<4;k++){
		printf("%d- 0x%08x\n",k,lb(0,k));
	}
	printf("Palavra 2:\n");
	for(k=0;k<4;k++){
		printf("%d- 0x%08x\n",k,lb(4,k));
	}
	printf("Palavra 1 unsigned:\n");
	for(k=0;k<4;k++){
		printf("%d- 0x%08x\n",k,lbu(0,k));
	}
	printf("Palavra 2 unsigned:\n");
	for(k=0;k<4;k++){
		printf("%d- 0x%08x\n",k,lbu(4,k));
	}

//	Inicialização da memória
	for(i=0;i < MEM_SIZE;i++){
		mem[i] = 0;
	}

	return 0;
}