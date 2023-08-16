/* 	Universidade de Brasilia
	Instituto de Ciencias Exatas
	Departamento de Ciencia da Computacao
	Algoritmos e Programação de Computadores – 2/2018
	Aluno(a): Vinicius Caixeta de Souza
	Matricula: 18/0132199
	Turma: A
	Versão do compilador: GCC
	Descricao: Este programa possui uma versao mais simples do jogo puyo-puyo,
	alem das instrucoes de como jogar.*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#ifndef _WIN32
	#include <termios.h>
	#include <fcntl.h>
	int kbhit(){
		struct termios oldt, newt;
		int ch, oldf;
		tcgetattr(STDIN_FILENO,&oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		fcntl(STDIN_FILENO, F_SETFL, oldf);
		if(ch != EOF){
		ungetc(ch,stdin);
		return 1;
		}
		return 0;
	}
	int getch(void) {
		int ch;
		struct termios oldt;
		struct termios newt;
		tcgetattr(STDIN_FILENO,&oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		return ch;
	}
	#else
		#include <conio.h>
	#endif
#ifdef _WIN32
	#define CLEAR "cls"
#else
	#define CLEAR "clear"
#endif
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define MENUMAIN_JOGAR 1
#define MENUMAIN_CONFIGURACOES 3
#define MENUMAIN_INSTRUCOES 2
#define MENUMAIN_RANKING 4
#define MENUMAIN_SAIR 5
#define UP 119
#define RIGHT 100
#define LEFT 97
#define DOWN 115
#define TROCAR 101
int largura, altura; /* Dimensoes do tabuleiro */
int qtdP, pontmax; /* quantidade maxima de pecas e combinacoes */
int velocidade; /* tempo em milissegundos do jogo */
int config[5]; /* vetor que armazena as configuracoes das variaveis acima */
char tabuleiro[100][100];
char p1, p2; /* Duas pecas do puyo-puyo */
int pontos;
char arqtxt[] = "configuracoes.txt";
int replayopcao; /* Determina se uma partida cria ou usa um replay */
char arqreplay[100], binrank[] = "ranking.bin";
int modoranking = 0; /* Determina se uma partida e rankeada */

/* Cria uma estrutura para o jogador */
typedef struct{
	char nick[11];
	int score;
} Player;

/* Cria o tabuleiro */
void criatabul(){
	int i, j;
	for(i=0; i<altura; i++){
		for(j=0; j<largura-1; j++){
			if((j==0) || (j==largura-2) || (i==altura-1)){
				tabuleiro[i][j] = '#';
			}
			else{
				tabuleiro[i][j] = ' ';
			}
		}
		tabuleiro[i][j] = '\0';
	}
}

/* Escolhe de forma randomica entre as letras 'A' ate a quantidade de pecas possíveis */
void criachar(int seek, int* continuarreplay){
	FILE* filereplay;
	if((replayopcao==1) || (modoranking==1)){

		p1 = 'A' + rand() % qtdP;
		p2 = 'A' + rand() % qtdP;
		if(modoranking==0){ /* Armazena as pecas em um arquivo texto para futuro replay */
			filereplay = fopen(arqreplay, "a");

			fprintf(filereplay, "%c%c", p1, p2);

			fclose(filereplay);
		}

	}
	if(replayopcao==2){ /* Procura repetir determinada partida */
		filereplay = fopen(arqreplay, "r");

		fseek(filereplay, seek, SEEK_SET);
		
		if(fscanf(filereplay, "%c%c", &p1, &p2) == EOF){
			*continuarreplay = 0;
		}

		fclose(filereplay);
	}
}

/*
#############################################
#Escolhe de forma randomica qual posicao as # 
#duas pecas vao comecar no tabuleiro, se    #
#elas nao comecarem em espaco livre a funcao#
#retorna 0 e o jogo acabara                 #
#############################################
*/
int posicaonotabul(){
	int posicao = rand() % (largura-4)+1;
	int i;

	for(i=1; i<(largura-3); i++){
		if(i == posicao){
			if((tabuleiro[0][i] != ' ') || (tabuleiro[0][i+1] != ' ')){ /* se ja existir uma peca no lugar ocorrera game over */
				return 0;
			}
			else{
				tabuleiro[0][i] = p1;
				tabuleiro[0][i+1] = p2;
				return i;
			}
		}
	}
	return 0;
}

/* Imprime o tabuleiro e a pontuacao */
void printjogo(int p){
	int i;
	printf("Pontos : %d\n\n", p);
	
	for(i=0; i<altura; i++){
		printf("%s\n", tabuleiro[i]);
	}
}

/* 
###############################
# Permite mudar as posicoes   #
#das pecas, mudar a velocidade#
#e alinhar horizontalmente ou #
#verticalmente                #
###############################
 */
void movimentacao(int* posicao, int i, int contp1, int contp2, int* usleepnormal, int* movhor){
	if(kbhit() == 1){
		int move = getch();
        
        /* Move as pecas para esquerda */
        if((move == LEFT) && (contp1 == 1) && (contp2 == 1)){
        	/* Na horizontal */
            if((*movhor == 1) && (tabuleiro[i][*posicao-1] == ' ')){
                tabuleiro[i][*posicao-1] = p1;
                tabuleiro[i][*posicao] = p2;
                tabuleiro[i][*posicao+1] = ' ';
                *posicao = *posicao - 1;
                system(CLEAR);
                printjogo(pontos);
            }
            /* Na vertical */
            else if((tabuleiro[i-1][*posicao-1] == ' ') && (tabuleiro[i][*posicao-1] == ' ')){
                tabuleiro[i-1][*posicao-1] = p2;
                tabuleiro[i][*posicao-1] = p1;
                tabuleiro[i][*posicao] = ' ';
                tabuleiro[i-1][*posicao] = ' ';
                *posicao = *posicao - 1;
                system(CLEAR);
                printjogo(pontos);
            }
        }
        
        /* Move as pecas para direita */
		if((move == RIGHT) && (contp1 == 1) && (contp2 == 1)){
            /* Na horizontal */
			if((*movhor == 1) && (tabuleiro[i][*posicao+2] == ' ')){
                tabuleiro[i][*posicao+2] = p2;
                tabuleiro[i][*posicao+1] = p1;
                tabuleiro[i][*posicao] = ' ';
                *posicao = *posicao + 1;
                system(CLEAR);
                printjogo(pontos);
            }
            /* Na vertical */
            else if((tabuleiro[i-1][*posicao+1] == ' ') && (tabuleiro[i][*posicao+1] == ' ')){
                tabuleiro[i-1][*posicao+1] = p2;
                tabuleiro[i][*posicao+1] = p1;
                tabuleiro[i-1][*posicao] = ' ';
                tabuleiro[i][*posicao] = ' ';
                *posicao = *posicao + 1;
                system(CLEAR);
                printjogo(pontos);
            }
        }
        
        /* Aumenta a velocidade das pecas caindo */
		if(move == DOWN){
            *usleepnormal = 0;
        }
        
        /* Troca a ordem das pecas */
        if((move == TROCAR) && (contp1 == 1) && (contp2 == 1)){
            int aux = p1;
            p1 = p2;
            p2 = aux;
            if(*movhor==1){
                tabuleiro[i][*posicao] = p1;
                tabuleiro[i][*posicao+1] = p2;
                system(CLEAR);
                printjogo(pontos);
            }
            else{
                tabuleiro[i][*posicao] = p1;
                tabuleiro[i-1][*posicao] = p2;
                system(CLEAR);
                printjogo(pontos);
            }
        }
        /* muda as pecas para horizontal ou vertical */
        if((move == UP) && (contp1 == 1) && (contp2 == 1) && (i>0)){
            if(*movhor==1){
                tabuleiro[i][*posicao+1] = ' ';
                tabuleiro[i-1][*posicao] = p2;
                *movhor = 0;
                system(CLEAR);
                printjogo(pontos);
            }
            else if(tabuleiro[i][*posicao+1] == ' '){
                tabuleiro[i][*posicao+1] = p2;
                tabuleiro[i-1][*posicao] = ' ';
                *movhor = 1;
                system(CLEAR);
                printjogo(pontos);
            }
        }
	}
}

/*
######################################
# Analisa os lados de uma letra e    #
# determina se havera uma combinacao #
# ou nao, se houver a matriz ira     #
# armazenar a posicao da combinacao  #
######################################
*/
void analisaradjacente(int i, int j, int tabuleiropontos[altura-1][largura-1], int excluirletra[20][20], int* adjacente, int* n){
	tabuleiropontos[i][j] = 0;
	excluirletra[*n][0] = i;
    excluirletra[*n][1] = j;
    *n = *n + 1;

	if(tabuleiropontos[i][j+1] == 1){
    	*adjacente = *adjacente + 1;
    	analisaradjacente(i, j+1, tabuleiropontos, excluirletra, adjacente, n);
    }
    if(tabuleiropontos[i][j-1] == 1){
    	*adjacente = *adjacente + 1;
    	analisaradjacente(i, j-1, tabuleiropontos, excluirletra, adjacente, n);
    }
    if(tabuleiropontos[i+1][j] == 1){
    	*adjacente = *adjacente + 1;
    	analisaradjacente(i+1, j, tabuleiropontos, excluirletra, adjacente, n);
    }
    if(tabuleiropontos[i-1][j] == 1){
    	*adjacente = *adjacente + 1;
    	analisaradjacente(i-1, j, tabuleiropontos, excluirletra, adjacente, n);
    }
}


/*
####################################
# Analisa uma letra e determina se #
# existe uma combinacao suficiente #
# para ganhar pontos               #
####################################
*/
void ganharpontosletras(int tabuleiropontos[altura-1][largura-1], int qtdP, char letras[qtdP], int k){
    int i, j, l;
    
    for(i=0; i<altura-1; i++){
        for(j=1; j<largura-2; j++){
            if(tabuleiro[i][j] == letras[k]){
                tabuleiropontos[i][j] = 1;
            }
        }
    }

    for(i=0; i<altura-1; i++){
    	for(j=1; j<largura-2; j++){
    		if(tabuleiropontos[i][j] == 1){

    			int adjacente = 1, n = 0;
    			int excluirletra[20][20]; /* Determina as posicoes em que as letras serao deletadas */
    			analisaradjacente(i, j, tabuleiropontos, excluirletra, &adjacente, &n);
    			if(adjacente>=pontmax){  /* se existir pontuacao necessaria os pontos aumentam e as letras somem */
    				pontos += adjacente;
    				if(adjacente==pontmax){
    					for(l=0; l<pontmax; l++){
    						tabuleiro[excluirletra[l][0]][excluirletra[l][1]] = ' ';
    					}
    				}
    				if(adjacente==pontmax+1){
    					for(l=0; l<pontmax+1; l++){
    						tabuleiro[excluirletra[l][0]][excluirletra[l][1]] = ' ';
    					}
    				}
    				if(adjacente==pontmax+2){
    					for(l=0; l<pontmax+2; l++){
    						tabuleiro[excluirletra[l][0]][excluirletra[l][1]] = ' ';
    					}
    				}
    			}
       		}
    	}
    }
}

/* Cria uma replica do tabuleiro com 0s */
void resettabuleiropontos(int tabuleiropontos[altura-1][largura-1]){
	int i, j;
	for(i=0; i<altura-1; i++){
        for(j=1; j<largura-1; j++){
            tabuleiropontos[i][j] = 0;
        }
    }
}

/* Analisa se a pontuacao ira aumentar por cada letra */
void ganharpontos(){
	int i = qtdP;
	char letras[10] = "ABCDEFGHIJ";
	
    int tabuleiropontos[altura-1][largura-1];

    for(i=0; i<qtdP; i++){
    	resettabuleiropontos(tabuleiropontos);
    	ganharpontosletras(tabuleiropontos, qtdP, letras, i);
    }
}

/* determina se a peca deve continuar a cair */
void pecanotabul(int posicao, int cont){
	int i, j;
	int contp1 = 1, contp2 = 1; /* Determina se cada peca ainda esta caindo */ 
	int usleepnormal = 1; /* Determina se as pecas estao caindo em velocidade normal */
    int movhor = 1; /* Define se as pecas estao movendo de forma horizontal ou vertical */

	for(i=0; i<altura-2 && cont!=2; i++){
        for(j=0; j<20; j++){
            if(usleepnormal == 1){
                usleep(velocidade); 
            }
            else{
                usleep(velocidade / 6);
            }
            movimentacao(&posicao, i, contp1, contp2, &usleepnormal, &movhor);
        }
        /* Analisa as pecas na horizontal */
        if(movhor == 1){
            if((tabuleiro[i+1][posicao] != ' ') && (contp1 == 1)){
                cont++;
                contp1 = 0;
            }
            if(tabuleiro[i+1][posicao] == ' '){
                tabuleiro[i+1][posicao] = p1;
                tabuleiro[i][posicao] = ' ';
            }
            if((tabuleiro[i+1][posicao+1] != ' ') && (contp2 == 1)){
                cont++;
                contp2 = 0;
            }
            if(tabuleiro[i+1][posicao+1] == ' '){
                tabuleiro[i+1][posicao+1] = p2;
                tabuleiro[i][posicao+1] = ' ';
            
            }
        }
        /* Analisa as pesa na vertical */
        else{
            if((tabuleiro[i+1][posicao] != ' ') && (contp1 == 1)){
                cont = cont + 2;
                contp1 = 0;
            }
            if(tabuleiro[i+1][posicao] == ' '){
                tabuleiro[i+1][posicao] = p1;
                tabuleiro[i][posicao] = p2;
                tabuleiro[i-1][posicao] = ' ';
            }
        }
		system(CLEAR);
		printjogo(pontos);	
	}
}

/* Le o arquivo texto com as configuracoes antes do jogo comecar */
void lerconfig(){
	FILE* filejogo;

	filejogo = fopen(arqtxt, "r");

	fscanf(filejogo, "%d %d %d %d %d", &altura, &largura, &qtdP, &pontmax, &velocidade);

	fclose(filejogo);
}

/* Escolhe entre criar um arquivo de replay ou usar um, se nao houver um usado dara erro */
void replay(){
	printf("1: Criar um arquivo de replay.\n");
	printf("2: Utilizar um arquivo de replay.\n");
	printf("Escolha uma das duas opcoes: ");
	scanf("%d", &replayopcao);

	if((replayopcao!=1) && (replayopcao!=2)){
		system(CLEAR);
		printf("Opcao invalida, tente novamente.\n");
		replay();
	}

	if(replayopcao==1){
		system(CLEAR);
		printf("Crie um nome para o arquivo: ");
		scanf("%s", arqreplay);
	}

	if(replayopcao==2){
		system(CLEAR);
		printf("Escolha um arquivo ja existente: ");
		scanf("%s", arqreplay);
		FILE* fd;

		fd = fopen(arqreplay, "r");
		if(fd == NULL){
			system(CLEAR);
			printf("Este arquivo não existe.\n");
			replay();
		}

		fclose(fd);
	}
}

/* Registra uma partida no modo rankeado em um arquivo binario */
void registerMatch(Player f){
	int i;
	FILE* fileranking;

	Player jogadorvazio[9];

	for(i=0; i<9; i++){
		jogadorvazio[i].nick[0] = '\0';
		jogadorvazio[i].score = 0;
	}

	fileranking = fopen(binrank, "rb");

	if(fileranking == NULL){
		fileranking = fopen(binrank, "wb");

		fwrite(&f, sizeof(Player), 1, fileranking);

		fwrite(&jogadorvazio, sizeof(Player), 9, fileranking);

		fclose(fileranking);
	}
	fclose(fileranking);
}

/*
#############################
# apresenta os componentes  #
# e funcoes para poder jogar#
# o jogo puyo-puyo          #
#############################
*/
void Jogar(){
	pontos = 0;
	lerconfig();
	int continuarjogando = 1, continuarreplay = 1;
	int seek = 0; /* Procura de duas em duas pecas no arquivo replay */
	Player jogador;

	system(CLEAR);
	srand(time(0));
	criatabul();

	if(modoranking==0){
		replay();
	}
	else{
		printf("Digite seu nome(min:1 ;max:10): ");
		scanf("%s", jogador.nick);
		int nicknum = strlen(jogador.nick);
		while((nicknum>10) || (nicknum<1)){
			system(CLEAR);
			printf("Digite seu nome(min:1 ;max:10): ");
			scanf("%s", jogador.nick);
			nicknum = strlen(jogador.nick);
		}
	}
	
	while(continuarjogando != 0){
		int cont = 0, posicao;
		criachar(seek, &continuarreplay);
		seek += 2;
		posicao = posicaonotabul();
		continuarjogando = posicao;
		if((continuarjogando == 0) || (continuarreplay == 0)){
			system(CLEAR);
			printf("=========\nGAME OVER\n=========\n");
            printf("PONTUACAO : %d\n\n", pontos);
			printf("Pressione ENTER para voltar ao menu principal.\n");

			if(modoranking==1){
				jogador.score = pontos;
				registerMatch(jogador);
			}

			getch();
			system(CLEAR);
			return;
		}
		else{
			printjogo(pontos);
			pecanotabul(posicao, cont);
			ganharpontos();
			system(CLEAR);
		}
	}
	
}

/*
#########################
#Seleciona a opcao que  #
#o jogador deseja e     #
#armazena o valor       #
#########################
*/
void menuMain(int* opt){
	printf("1 - Jogar\n");
	printf("2 - Instrucoes\n");
	printf("3 - Configuracoes\n");
	printf("4 - Ranking\n");
	printf("5 - Sair\n");
	printf("Digite o numero correspondente a opcao desejada: ");
	scanf("%d", opt);
	printf("\n");
}

/* Mostra as instrucoes para o jogador */
void Instrucoes(){
	system(CLEAR);
	printf(YELLOW "Controles:" RESET "\n");
	printf("-tecla 'w': muda o bloco para vertical ou horizontal.\n");
	printf("-tecla 's': aumenta a velocidade em que o bloco cai.\n");
	printf("-tecla 'd'/'a': move o bloco para direita/esquerda.\n");
	printf("-tecla 'e': muda a ordem das pecas no bloco.\n");
	printf(YELLOW "Como jogar:" RESET "\n");
	printf("-Alinhe os blocos para que as letras fiquem juntas, quando 4 ou mais letras se\n");
	printf("juntarem elas desapareceram e a pontuacao aumentara de acordo com a quantidade\n");
	printf("que desapareceu.\n");
	printf("-Diferente de tetris os blocos caem ao inves de ficarem no lugar.\n");
	printf("-Quanto mais combinacoes ocorrerem, maior a pontuacao sera.\n\n");
}

/* Habilita a troca dos valores de variaveis relacionadas ao tabuleiro */
void configtabul(){
	FILE* fileconfig2;

	modoranking = 0;
	printf("\n");
	printf("Dimensoes do tabuleiro.\n");
	printf("Altura: ");
	scanf("%d", &config[0]);
	printf("Largura: ");
	scanf("%d", &config[1]);
	config[1] = config[1] + 1;

	fileconfig2 = fopen(arqtxt, "w");

	fprintf(fileconfig2, "%d %d %d %d %d", config[0], config[1], config[2], config[3], config[4], config[5]);

	fclose(fileconfig2);

	system(CLEAR);

	return;
}

/* Habilita a troca de valores de variaveis relacionadas as pecas de acordo com seus limites */
void configpecas(){
	FILE* fileconfig1;

	modoranking = 0;
	printf("\n");
	printf("Quantidade de letras no jogo(Min:2; Max:10; default:5): ");
	scanf("%d", &config[2]);
	while((config[2]>10) || (config[2]<2)){
		system(CLEAR);
		printf("Quantidade de letras invalida, repita a operacao.\n");
		scanf("%d", &config[2]);
	}

	printf("quantidade de combinacoes necessarias para pontuar(Min:3; Max:10; default:4): ");
	scanf("%d", &config[3]);
	while((config[3]>10) || (config[3]<3)){
		system(CLEAR);
		printf("Quantidade de combinacoes invalida, repita a operacao.\n");
		scanf("%d", &config[3]);
	}

	printf("Velocidade do jogo em milissegundos(default: 30000): ");
	scanf("%d", &config[4]);

	fileconfig1 = fopen(arqtxt, "w");

	fprintf(fileconfig1, "%d %d %d %d %d", config[0], config[1], config[2], config[3], config[4], config[5]);

	fclose(fileconfig1);

	system(CLEAR);

	return;
}

/* Apresenta as possiveis configuracoes que podem ser modificadas e as armazenam em um arquivo texto */
void Configuracoes(){
	system(CLEAR);
	int opcoes;
	FILE* fileconfig;

	printf("1.Tabuleiro\n");
	printf("2.Pecas\n");
	printf("3.Ativar Modo Rankeado\n");
	printf("4.Voltar\n");
	printf("Escolha uma das opcoes: ");
	scanf("%d", &opcoes);

	if((opcoes > 4) || (opcoes < 1)){
		system(CLEAR);
		printf("Opcao invalida, tente novamente.\n");
		Configuracoes();
	}

	if(opcoes == 4){
		system(CLEAR);
	}

	if(opcoes == 1){
		configtabul();
	}

	if(opcoes == 2){
		configpecas();
	}

	if(opcoes == 3){
		printf("\n");
		printf("1. Ativar modo ranking.\n");
		printf("2. Desativar modo ranking.\n");
		printf("Escolha uma das opcoes: ");
		scanf("%d", &modoranking);

		if(modoranking == 1){
			fileconfig = fopen(arqtxt, "w");

			fprintf(fileconfig, "%d %d %d %d %d", 15, 10, 7, 5, 30000);

			fclose(fileconfig);
		}
		if(modoranking==2){
			modoranking = 0;
		}

		Configuracoes();
	}
}

/* Apresenta o ranking */
void Ranking(){
	int i;
	system(CLEAR);
	FILE* rank;

	Player jogadores;

	rank = fopen(binrank, "rb");

	if(rank == NULL){
		printf("Nao existe nenhum rank, ative o modoranking para criar um.\n\n");
		return;
	}

	for(i=0; i<10; i++){
		fread(&jogadores, sizeof(Player), 1, rank);
		printf("%d. %s %d\n", i+1, jogadores.nick, jogadores.score);
	}

	fclose(rank);

	printf("Pressione <ENTER> para continuar.\n\n");
	getchar();
}

/*
########################
#detecta qual opcao foi#
#selecionada e ativa a #
#respectiva funcao     #
#relacionada           #
########################
*/
void menuMainOpt(int opt){
	if(opt == MENUMAIN_JOGAR){
		Jogar();
	}
	else if(opt == MENUMAIN_INSTRUCOES){
		Instrucoes();
	}
	else if(opt == MENUMAIN_CONFIGURACOES){
		Configuracoes();
	}
	else if(opt == MENUMAIN_RANKING){
		Ranking();
	}
	else{
		exit(EXIT_FAILURE);
	}
}

/* Configuracoes da primeira parte do trabalho em um arquivo texto */
void Configuracoesdefault(){
	FILE* filedefault;

	filedefault = fopen(arqtxt, "w");

	fprintf(filedefault, "%d %d %d %d %d", 13, 9, 5, 4, 30000);

	fclose(filedefault);
}

/* Principal funcao */
int main(){
	system(CLEAR);
	int opt;
	config[0] = 13;
	config[1] = 9;
	config[2] = 5;
	config[3] = 4;
	config[4] = 30000;
	Configuracoesdefault();

	printf("Bem-vindo(a) a:\n" GREEN "=========\n" RED "PUYO" CYAN "-" BLUE "PUYO\n" YELLOW "=========" RESET "\n");
	printf("Pressione enter.\n");
	getchar();
	system(CLEAR);

	while(opt!= MENUMAIN_SAIR){
		menuMain(&opt);
		menuMainOpt(opt);
	}

	return 0;
}
