#include <stdio.h>   // funções para mostrar mensagens e mexer com arquivos
#include <stdlib.h>  // funções de memória, encerramento e conversão de números
#include <string.h>  // funções usadas para trabalhar com textos
#include <ctype.h>   // funções para tratar letras e espaços

#define ARQUIVO        "loja.txt"   // arquivo onde ficam os produtos cadastrados
#define TEMPORARIO     "temp.txt"   // arquivo de apoio usado nas alterações
#define ARQUIVO_CSV    "loja.csv"   // arquivo criado na exportação
#define TAM_PRODUTO    50           // limite reservado para o nome
#define TAM_CATEGORIA  40           // espaço reservado para a categoria
#define TAM_FORNECEDOR 50           // espaço reservado para o fornecedor
#define TAM_LINHA      256          // quantidade de caracteres da leitura

// Aqui ficam as informações que cada produto precisa ter
typedef struct{
    int   id;                       // código usado para identificar cada produto
    char  produto[TAM_PRODUTO];     // guarda a descrição do produto
    char  categoria[TAM_CATEGORIA]; // grupo ao qual o produto pertence
    float preco;                    // valor de venda do produto
    int   quantidade;               // unidades disponíveis na loja
    char  fornecedor[TAM_FORNECEDOR]; // empresa que fornece o produto
} Produto;

//PARTE DE LEITURA E TRATAMENTO DOS DADOS
// Retira os espaços e quebras de linha que sobraram no texto
void removerEspacos(char *s){
    char *ini = s;                                     // começa apontando para o início do texto
    while(*ini != '\0' && isspace((unsigned char)*ini)) ini++; // ignora os espaços do começo
    char *fim = ini + strlen(ini);                     // localiza onde a string termina
    while(fim > ini && isspace((unsigned char)*(fim - 1))) fim--; // volta até encontrar um caractere válido
    *fim = '\0';                                       // coloca o fim do texto nessa posição
    memmove(s, ini, strlen(ini) + 1);                  // move o conteúdo sem os espaços para o começo
}

// Recebe uma entrada do usuário e salva o texto já tratado
void lerTexto(const char *mensagem, char *dest, int tam){
    char buf[TAM_LINHA];                               // espaço onde a entrada fica primeiro
    printf("%s", mensagem);                            // apresenta a mensagem no terminal
    if(fgets(buf, sizeof(buf), stdin) == NULL){      // verifica se ainda foi possível ler a entrada
        printf("\nEntrada encerrada.\n");              // informa que a leitura terminou
        exit(0);                                       // finaliza o programa caso não exista mais entrada
    }
    if(strchr(buf, '\n') == NULL){                   // confere se o usuário digitou além do limite
        int c;                                         // variável usada para limpar a entrada
        while((c = getchar()) != '\n' && c != EOF){} // limpa os caracteres que não couberam
    }
    for(char *p = buf; *p != '\0'; p++){             // passa por cada caractere informado
        if(*p == '|') *p = ' ';                       // troca esse símbolo para não atrapalhar a gravação
    }
    removerEspacos(buf);                               // faz a limpeza da entrada
    strncpy(dest, buf, tam - 1);                       // transfere o texto respeitando o limite
    dest[tam - 1] = '\0';                              // marca o fim da informação armazenada
}

// Obriga o preenchimento do campo antes de continuar
void lerTextoObrigatorio(const char *mensagem, char *dest, int tam){
    do{
        lerTexto(mensagem, dest, tam);                 // recebe o conteúdo digitado
        if(dest[0] == '\0')
            printf("Este campo não pode ficar vazio.\n"); // mostra o aviso quando falta preencher
    } 
    while (dest[0] == '\0');                         // continua perguntando até receber uma resposta
}

// Tenta transformar a entrada em número inteiro e informa se conseguiu
int converterInteiro(const char *texto, int *valor){
    char *fim;                                         // marca o ponto final da leitura do número
    long v = strtol(texto, &fim, 10);                  // transforma o texto em um número decimal inteiro
    if(texto[0] == '\0' || *fim != '\0') return 0;    // rejeita entradas vazias ou com caracteres extras
    *valor = (int)v;                                   // salva o número no endereço recebido
    return 1;                                          // informa que conseguiu converter
}

// Aceita um preço digitado com vírgula ou ponto e converte para decimal
int converterFloat(const char *texto, float *valor){
    char copia[TAM_LINHA];                             // guarda uma versão editável do texto
    strncpy(copia, texto, sizeof(copia) - 1);          // passa a entrada para a variável auxiliar
    copia[sizeof(copia) - 1] = '\0';                   // coloca o fim da string corretamente
    for(char *p = copia; *p != '\0'; p++){           // verifica cada posição do texto copiado
        if(*p == ',') *p = '.';                       // padroniza o separador decimal
    }
    char *fim;                                         // aponta para o final do número reconhecido
    double v = strtod(copia, &fim);                    // interpreta a entrada como um valor com casas decimais
    if(copia[0] == '\0' || *fim != '\0') return 0;    // não aceita texto vazio nem caracteres indevidos
    *valor = (float)v;                                 // salva o número no endereço recebido
    return 1;                                          // informa que conseguiu converter
}

// Pede um número inteiro e só aceita valores a partir do mínimo
int lerInteiro(const char *mensagem, int minimo){
    char buf[TAM_LINHA];                               // recebe o que foi informado pelo usuário
    int valor;                                         // guarda o resultado numérico
    while(1){                                        // mantém a pergunta até a entrada ser aceita
        lerTexto(mensagem, buf, sizeof(buf));          // recebe a entrada pelo teclado
        if(converterInteiro(buf, &valor) && valor >= minimo) return valor; // retorna o número quando passar na validação
        printf("Valor inválido. Digite um número inteiro >= %d.\n", minimo); // informa que precisa digitar novamente
    }
}

// Solicita um valor decimal que não pode ser negativo
float lerFloat(const char *mensagem){
    char buf[TAM_LINHA];                               // recebe o que foi informado pelo usuário
    float valor;                                       // guarda o resultado numérico
    while (1){                                        // mantém a pergunta até a entrada ser aceita
        lerTexto(mensagem, buf, sizeof(buf));          // recebe a entrada pelo teclado
        if(converterFloat(buf, &valor) && valor >= 0) return valor; // retorna o número quando passar na validação
        printf("Valor inválido. Digite um número >= 0 (ex.: 49,90).\n"); // informa que precisa digitar novamente
    }
}

// Verifica se o usuário confirmou a operação
int confirmar(const char *mensagem){
    char buf[TAM_LINHA];                               // armazena a confirmação informada
    lerTexto(mensagem, buf, sizeof(buf));              // recebe a escolha do usuário
    return (buf[0] == 's' || buf[0] == 'S');           // considera confirmação quando a primeira letra é S
}

// Deixa o texto em letras minúsculas para facilitar as comparações.
// Também trata algumas letras com acento, que precisam de um cuidado extra no UTF-8.
void paraMinusculo(const char *origem, char *destino, int tam){
    int j = 0;                                         // controla em qual posição o resultado será colocado
    for(const unsigned char *p = (const unsigned char *)origem; *p != '\0' && j < tam - 2; p++){
        if(*p == 0xC3 && *(p + 1) >= 0x80 && *(p + 1) <= 0x9E && *(p + 1) != 0x97){
            destino[j++] = (char)0xC3;                 // conserva a primeira parte do caractere acentuado
            destino[j++] = (char)(*(p + 1) + 0x20);    // ajusta a segunda parte para formar a letra minúscula
            p++;                                       // evita processar novamente esse byte
        } else{
            destino[j++] = (char)tolower(*p);          // converte normalmente as letras sem acento
        }
    }
    destino[j] = '\0';                                 // finaliza o texto convertido
}

// Compara duas informações sem considerar diferença entre maiúsculas e minúsculas
int compararTexto(const char *a, const char *b){
    char ma[TAM_LINHA], mb[TAM_LINHA];                 // versões padronizadas para fazer a comparação
    paraMinusculo(a, ma, sizeof(ma));                  // transforma o primeiro texto
    paraMinusculo(b, mb, sizeof(mb));                  // transforma o segundo texto
    return strcmp(ma, mb);                             // verifica a ordem dos dois textos
}

// Procura uma palavra ou parte dela sem diferenciar letras maiúsculas
int contemTexto(const char *texto, const char *trecho){
    char mt[TAM_LINHA], mr[TAM_LINHA];                 // versões padronizadas para fazer a comparação
    paraMinusculo(texto, mt, sizeof(mt));              // padroniza a informação completa
    paraMinusculo(trecho, mr, sizeof(mr));             // padroniza a palavra procurada
    return strstr(mt, mr) != NULL;                     // confirma se o trecho foi localizado
}

// Calcula quantas letras aparecem de verdade, mesmo com acentos
int tamanhoVisual(const char *s){
    int n = 0;                                         // total de posições visíveis
    for (; *s != '\0'; s++) {                          // analisa os caracteres do texto
        if (((unsigned char)*s & 0xC0) != 0x80) n++;   // não soma os bytes extras usados nos acentos
    }
    return n;                                          // devolve o tamanho que será mostrado
}

// Mostra o conteúdo com os espaços necessários para alinhar a tabela
void imprimirCampo(const char *s, int largura){
    printf("%s", s);                                   // apresenta o conteúdo do campo
    for(int i = tamanhoVisual(s); i < largura; i++) putchar(' '); // preenche o espaço restante da coluna
}

//  PARTE DOS ARQUIVOS E DOS PRODUTOS
// Inverte as posições de dois produtos com auxílio dos ponteiros
void trocar(Produto *a, Produto *b){
    Produto temp = *a;                                 // deixa o primeiro produto guardado temporariamente
    *a = *b;                                           // coloca o segundo produto na primeira posição
    *b = temp;                                         // termina a troca usando o valor salvo
}

// Salva cada cadastro em uma linha com os campos separados pelo símbolo |
void gravarProduto(FILE *fp, const Produto *p){
    fprintf(fp, "%d|%s|%s|%.2f|%d|%s\n",              // organiza os valores no padrão do arquivo de dados
            p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Carrega os cadastros do arquivo para uma lista criada na memória
// Caso não consiga carregar, retorna NULL; a memória obtida precisa ser liberada depois.
Produto *carregarProdutos(const char *arquivo, int *qtd){
    *qtd = 0;                                          // zera a quantidade antes da leitura
    FILE *fp = fopen(arquivo, "r");                    // tenta acessar os dados salvos
    if(fp == NULL) return NULL;                       // não conseguiu acessar o arquivo informado

    char linha[TAM_LINHA];                             // espaço usado para ler cada registro
    int linhas = 0;                                    // conta quantos registros podem existir
    while(fgets(linha, sizeof(linha), fp) != NULL) linhas++; // primeiro verifica o tamanho necessário
    rewind(fp);                                        // retorna para reler desde a primeira linha

    Produto *lista = (Produto *)malloc((linhas + 1) * sizeof(Produto)); // reserva memória para os registros encontrados
    if(lista == NULL){                               // verifica se a reserva de memória deu errado
        fclose(fp);                                    // encerra a leitura antes de retornar
        return NULL;                                   // devolve NULL para indicar que não foi possível continuar
    }

    while(fgets(linha, sizeof(linha), fp) != NULL){  // agora pega os dados de cada registro
        Produto p;                                     // guarda um cadastro durante a leitura
        // limita o tamanho dos campos e separa as informações pelo |
        int lidos = sscanf(linha, "%d|%49[^|]|%39[^|]|%f|%d|%49[^\r\n]",
                           &p.id, p.produto, p.categoria, &p.preco, &p.quantidade, p.fornecedor);
        if (lidos == 6) {                              // descarta registros que não tenham todas as informações
            *(lista + *qtd) = p;                       // adiciona o produto na posição disponível
            (*qtd)++;                                  // contabiliza mais um produto carregado
        }
    }
    fclose(fp);                                        // encerra o acesso ao arquivo
    return lista;                                      // retorna os produtos que foram lidos
}

// Tenta abrir os cadastros e informa quando o arquivo não está disponível
Produto *carregarOuAvisar(int *qtd){
    Produto *lista = carregarProdutos(ARQUIVO, qtd);   // busca os dados cadastrados
    if (lista == NULL){                               // verifica se a leitura falhou
        printf("O arquivo %s não existe. Use a opção 1 para criá-lo.\n", ARQUIVO);
    }
    return lista;                                      // retorna o resultado da tentativa de leitura
}

// Reescreve os cadastros em um arquivo auxiliar antes de trocar o principal.
// A troca é feita depois da gravação do temporário, mas uma falha ao renomear ainda pode deixar o original ausente.
int substituirArquivo(const Produto *lista, int qtd){
    FILE *temp = fopen(TEMPORARIO, "w");               // abre o arquivo auxiliar para escrita
    if(temp == NULL){                                // confere se foi possível criar esse arquivo
        printf("Erro ao criar o arquivo temporário!\n");
        return 0;                                      // indica que a operação não foi concluída
    }
    for(int i = 0; i < qtd; i++){                    // passa pelos cadastros da lista
        gravarProduto(temp, lista + i);                // escreve os registros no arquivo auxiliar
    }
    fclose(temp);                                      // encerra a escrita no arquivo de apoio
    remove(ARQUIVO);                                   // remove o arquivo antigo antes da troca
    if(rename(TEMPORARIO, ARQUIVO) != 0){            // tenta colocar o arquivo novo no lugar do antigo
        printf("Erro ao substituir o arquivo original!\n");
        return 0;                                      // indica que a operação não foi concluída
    }
    return 1;                                          // informa que a substituição terminou
}

// Procura um produto pelo código e devolve sua posição na memória
Produto *buscarPorId(Produto *lista, int qtd, int id){
    for(int i = 0; i < qtd; i++){                    // verifica os elementos da lista
        if((lista + i)->id == id) return lista + i;   // retorna onde está o cadastro encontrado
    }
    return NULL;                                       // retorna NULL quando não encontra o código
}

// Define o próximo código com base no maior ID já cadastrado
int gerarId(const Produto *lista, int qtd) {
    int maior = 0;                                     // valor inicial para encontrar o maior código
    for(int i = 0; i < qtd; i++){                    // verifica os elementos da lista
        if((lista + i)->id > maior) maior = (lista + i)->id; // atualiza o maior identificador encontrado
    }
    return maior + 1;                                  // devolve o número seguinte ao maior ID
}

// Confere se o nome informado já pertence a outro cadastro
// Na edição, desconsidera o próprio ID para não acusar duplicidade.
int nomeDuplicado(const Produto *lista, int qtd, const char *nome, int idIgnorar){
    for(int i = 0; i < qtd; i++) {                    // verifica os elementos da lista
        if((lista + i)->id != idIgnorar && compararTexto((lista + i)->produto, nome) == 0){
            return 1;                                  // encontrou outro cadastro com esse nome
        }
    }
    return 0;                                          // não existe outro produto com esse nome
}

// APRESENTAÇÃO DOS PRODUTOS

// Organiza os cadastros em colunas apenas para mostrar na tela
void exibirTabela(const Produto *lista, int qtd) {
    if(qtd == 0){                                    // não há registros para exibir
        printf("Nenhum produto encontrado.\n");
        return;
    }
    int lp = 7, lc = 9, lf = 10;                       // define o espaço inicial de cada coluna
    for(int i = 0; i < qtd; i++){                    // calcula o espaço necessário para os conteúdos
        int t;                                         // mede o conteúdo sem contar bytes extras
        t = tamanhoVisual((lista + i)->produto);   if (t > lp) lp = t; // aumenta a coluna se o nome precisar
        t = tamanhoVisual((lista + i)->categoria); if (t > lc) lc = t; // amplia a coluna quando necessário
        t = tamanhoVisual((lista + i)->fornecedor); if (t > lf) lf = t; // ajusta o espaço para o fornecedor
    }
    int total = 4 + 3 + lp + 3 + lc + 3 + 10 + 3 + 10 + 3 + lf + 2; // calcula o comprimento do separador
    for(int i = 0; i < total; i++) putchar('-');      // desenha a borda de cima
    printf("\n| ID   | ");                             // começa a primeira linha da tabela
    imprimirCampo("Produto", lp);   printf(" | ");     // apresenta o campo de produtos
    imprimirCampo("Categoria", lc); printf(" | ");     // apresenta o campo de categorias
    printf("Preço (R$) | Quantidade | ");              // apresenta as informações de valor e estoque
    imprimirCampo("Fornecedor", lf); printf(" |\n");   // apresenta a coluna do fornecedor
    for(int i = 0; i < total; i++) putchar('-');      // separa o título dos registros
    putchar('\n');
    for(int i = 0; i < qtd; i++){                    // imprime um cadastro por vez
        const Produto *p = lista + i;                  // acessa o registro desta posição
        printf("| %4d | ", p->id);                     // mostra o código na largura reservada
        imprimirCampo(p->produto, lp);   printf(" | "); // apresenta o nome
        imprimirCampo(p->categoria, lc); printf(" | "); // apresenta a categoria
        printf("%10.2f | %10d | ", p->preco, p->quantidade); // exibe o preço junto das unidades em estoque
        imprimirCampo(p->fornecedor, lf); printf(" |\n");    // apresenta o fornecedor
    }
    for(int i = 0; i < total; i++) putchar('-');      // fecha a tabela com um separador
    printf("\n%d produto(s).\n", qtd);                 // informa quantos cadastros apareceram
}

// FUNCIONALIDADES DO CADASTRO

// 1. CRIAR ARQUIVO: começa a loja com os quatro produtos do exercício
void criarArquivo(void) {
    FILE *teste = fopen(ARQUIVO, "r");                 // confere se o arquivo já foi criado
    if (teste != NULL){                               // já encontrou um arquivo anterior
        fclose(teste);                                 // fecha a verificação inicial
        if(!confirmar("O arquivo já existe e será recriado do zero. Continuar? (s/n): ")){
            printf("Operação cancelada.\n");
            return;                                    // interrompe sem recriar o arquivo
        }
    }
    Produto iniciais[4] = {                            // produtos iniciais fornecidos na atividade
        {1, "Camiseta Básica",  "Vestuário",       49.90f, 30, "Moda Brasil"},
        {2, "Mouse Óptico USB", "Informática",     59.99f, 15, "TecnoShop"},
        {3, "Shampoo 300ml",    "Higiene Pessoal", 22.50f, 40, "Beleza Pura"},
        {4, "Café 500g",        "Alimentos",       18.00f, 25, "Grãos do Sul"}
    };
    FILE *fp = fopen(ARQUIVO, "w");                    // abre para escrever do zero, substituindo o conteúdo anterior
    if(fp == NULL){                                  // verifica se a criação falhou
        printf("Erro ao criar o arquivo!\n");
        return;
    }
    for(int i = 0; i < 4; i++){                      // passa pelos quatro cadastros iniciais
        gravarProduto(fp, iniciais + i);               // salva cada produto separadamente
    }
    fclose(fp);                                        // encerra o acesso ao arquivo
    printf("Arquivo %s criado com 4 produtos iniciais.\n", ARQUIVO);
}

// 2. LISTAR: apresenta os produtos que estão salvos
void listarProdutos(void){
    int qtd;                                           // total encontrado no arquivo
    Produto *lista = carregarOuAvisar(&qtd);           // recupera os registros armazenados
    if(lista == NULL) return;                         // interrompe se não houver arquivo disponível
    printf("\n=== LISTA DE PRODUTOS ===\n");
    exibirTabela(lista, qtd);                          // apresenta os registros organizados
    free(lista);                                       // devolve a memória utilizada
}

// 3. INSERIR: adiciona um produto e cria seu código automaticamente
void inserirProduto(void){
    int qtd;                                           // total de registros antes da inclusão
    Produto *lista = carregarOuAvisar(&qtd);           // busca o que já está cadastrado
    if(lista == NULL) return;                         // interrompe se não houver arquivo disponível

    Produto novo;                                      // guarda os dados da nova inclusão
    lerTextoObrigatorio("Nome do produto: ", novo.produto, TAM_PRODUTO); // solicita a identificação do produto
    if(nomeDuplicado(lista, qtd, novo.produto, 0)){  // confere se esse nome já está sendo usado
        printf("Já existe um produto com o nome \"%s\". Cadastro cancelado.\n", novo.produto);
        free(lista);                                   // limpa os dados carregados da memória
        return;
    }
    lerTextoObrigatorio("Categoria: ", novo.categoria, TAM_CATEGORIA);     // pergunta qual é o grupo do produto
    novo.preco = lerFloat("Preço (R$): ");             // recebe o valor de venda
    novo.quantidade = lerInteiro("Quantidade: ", 0);   // recebe o número de unidades
    lerTextoObrigatorio("Fornecedor: ", novo.fornecedor, TAM_FORNECEDOR);  // solicita a empresa fornecedora

    novo.id = gerarId(lista, qtd);                     // define o código sem precisar pedir ao usuário
    while(buscarPorId(lista, qtd, novo.id) != NULL){ // confirma que esse código não está em uso
        novo.id++;                                     // avança para outro ID disponível
    }

    FILE *fp = fopen(ARQUIVO, "a");                    // abre para adicionar um cadastro ao fim do arquivo
    if(fp == NULL){                                  // trata o caso em que o arquivo não abriu
        printf("Erro ao abrir o arquivo!\n");
        free(lista);
        return;
    }
    gravarProduto(fp, &novo);                          // adiciona os dados do cadastro
    fclose(fp);                                        // encerra o acesso ao arquivo
    printf("Produto cadastrado com sucesso! ID gerado: %d\n", novo.id);
    free(lista);                                       // limpa os dados carregados da memória
}

// 4. ALTERAR: permite editar um produto sem preencher tudo novamente
void alterarProduto(void){
    int qtd;                                           // total de produtos registrados
    Produto *lista = carregarOuAvisar(&qtd);           // lê as informações da loja
    if(lista == NULL) return;                         // interrompe se não houver arquivo disponível

    int id = lerInteiro("ID do produto a alterar: ", 1); // solicita o código do cadastro
    Produto *p = buscarPorId(lista, qtd, id);          // localiza o produto escolhido
    if(p == NULL){                                   // o código informado não está cadastrado
        printf("Produto com ID %d não encontrado.\n", id);
        free(lista);
        return;
    }

    printf("Produto atual: %s | %s | R$ %.2f | %d un. | %s\n",
           p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
    printf("(Deixe em branco e tecle Enter para manter o valor atual)\n");

    Produto novo = *p;                                 // separa uma cópia para receber as alterações
    char buf[TAM_LINHA];                               // recebe o que foi informado pelo usuário

    lerTexto("Novo nome: ", buf, TAM_PRODUTO);         // pergunta se o nome será alterado
    if(buf[0] != '\0'){                              // só continua a mudança quando houver conteúdo
        if(nomeDuplicado(lista, qtd, buf, id)){      // procura nomes repetidos entre os outros registros
            printf("Já existe outro produto com esse nome. Alteração cancelada.\n");
            free(lista);
            return;
        }
        strcpy(novo.produto, buf);                     // troca o nome na cópia
    }

    lerTexto("Nova categoria: ", buf, TAM_CATEGORIA);  // solicita outra categoria, se necessário
    if(buf[0] != '\0') strcpy(novo.categoria, buf);   // conserva o valor anterior quando ficar vazio

    while(1){                                        // pede um preço novo ou mantém o que já estava
        lerTexto("Novo preço (R$): ", buf, sizeof(buf));
        if (buf[0] == '\0') break;                     // não altera o preço quando pressiona Enter
        float v;                                       // número obtido da entrada
        if(converterFloat(buf, &v) && v >= 0){ novo.preco = v; break; } // atualiza a informação depois da validação
        printf("Valor inválido.\n");
    }

    while(1){                                        // recebe a quantidade nova ou mantém a atual
        lerTexto("Nova quantidade: ", buf, sizeof(buf));
        if (buf[0] == '\0') break;                     // não modifica o estoque se estiver em branco
        int v;                                         // número obtido da entrada
        if(converterInteiro(buf, &v) && v >= 0){ novo.quantidade = v; break; } // atualiza a informação depois da validação
        printf("Valor inválido.\n");
    }

    lerTexto("Novo fornecedor: ", buf, TAM_FORNECEDOR); // permite atualizar a empresa fornecedora
    if(buf[0] != '\0') strcpy(novo.fornecedor, buf);  // conserva o valor anterior quando ficar vazio

    *p = novo;                                         // passa a versão editada para a lista
    if(substituirArquivo(lista, qtd)){               // atualiza os registros usando o arquivo auxiliar
        printf("Produto alterado com sucesso!\n");
    }
    free(lista);                                       // limpa os dados carregados da memória
}

// 5. EXCLUIR: apaga um cadastro pelo código informado
void excluirProduto(void){
    int qtd;                                           // total de produtos registrados
    Produto *lista = carregarOuAvisar(&qtd);           // lê as informações da loja
    if (lista == NULL) return;                         // interrompe se não houver arquivo disponível

    int id = lerInteiro("ID do produto a excluir: ", 1); // solicita o código do cadastro
    Produto *p = buscarPorId(lista, qtd, id);          // localiza o produto escolhido
    if(p == NULL){                                   // o código informado não está cadastrado
        printf("Produto com ID %d não encontrado.\n", id);
        free(lista);
        return;
    }

    printf("Produto: %s | %s | R$ %.2f\n", p->produto, p->categoria, p->preco);
    if(!confirmar("Confirma a exclusão? (s/n): ")){  // confirma antes de remover definitivamente
        printf("Exclusão cancelada.\n");
        free(lista);
        return;
    }

    for(Produto *q = p; q < lista + qtd - 1; q++){   // desloca os próximos registros para preencher o espaço
        *q = *(q + 1);                                 // coloca o registro seguinte na posição atual
    }
    qtd--;                                             // reduz o total depois da exclusão

    if(substituirArquivo(lista, qtd)){               // atualiza os registros usando o arquivo auxiliar
        printf("Produto excluído com sucesso!\n");
    }
    free(lista);                                       // limpa os dados carregados da memória
}

// 6 e 7. BUSCAS: encontra produtos pelo nome ou pela categoria
void buscarProdutos(int porNome) {
    int qtd;                                           // total de produtos registrados
    Produto *lista = carregarOuAvisar(&qtd);           // lê as informações da loja
    if (lista == NULL) return;                         // interrompe se não houver arquivo disponível

    char termo[TAM_PRODUTO];                           // palavra usada na pesquisa
    lerTextoObrigatorio(porNome ? "Nome (ou parte do nome): " : "Categoria (ou parte): ",
                        termo, sizeof(termo));         // escolhe a pergunta de acordo com a opção

    Produto *achados = (Produto *)malloc((qtd + 1) * sizeof(Produto)); // reserva espaço para os produtos encontrados
    if(achados == NULL){                             // verifica se a reserva de memória deu errado
        free(lista);
        return;
    }
    int n = 0;                                         // conta os cadastros encontrados
    for(int i = 0; i < qtd; i++){                    // passa pelos cadastros da lista
        const char *campo = porNome ? (lista + i)->produto : (lista + i)->categoria; // escolhe entre o nome e a categoria
        if(contemTexto(campo, termo)){               // procura sem diferenciar letras grandes e pequenas
            *(achados + n) = *(lista + i);             // adiciona o cadastro encontrado à lista de pesquisa
            n++;                                       // soma mais um item encontrado
        }
    }
    printf("\n=== RESULTADO DA BUSCA POR %s: \"%s\" ===\n", porNome ? "NOME" : "CATEGORIA", termo);
    exibirTabela(achados, n);                          // exibe apenas os registros que combinaram
    free(achados);                                     // libera a lista usada na pesquisa
    free(lista);                                       // encerra o uso da lista original
}

// Verifica se dois registros precisam inverter de posição na ordenação
// O critério escolhe nome ou preço, e crescente define a direção.
int deveTrocar(const Produto *a, const Produto *b, int criterio, int crescente){
    int cmp;                                           // guarda se o primeiro vem antes, depois ou é igual
    if(criterio == 1){                               // compara pela descrição do produto
        cmp = compararTexto(a->produto, b->produto);   // usa os nomes padronizados na comparação
    } else {                                           // compara os valores dos produtos
        cmp = (a->preco > b->preco) - (a->preco < b->preco); // converte a comparação em um resultado numérico
    }
    return crescente ? (cmp > 0) : (cmp < 0);          // verifica a posição certa para a ordem escolhida
}

// Usa Bubble Sort para organizar a lista de produtos
void bubbleSort(Produto *lista, int n, int criterio, int crescente){
    for (int i = 0; i < n - 1; i++) {                  // repete as verificações ao longo da lista
        for (int j = 0; j < n - i - 1; j++) {          // analisa cada dupla e vai levando os elementos para a posição correta
            if(deveTrocar(lista + j, lista + j + 1, criterio, crescente)){
                trocar(lista + j, lista + j + 1);      // inverte os cadastros quando necessário
            }
        }
    }
}

void ordenarProdutos(int criterio, int crescente){
    int qtd;                                           // total de produtos registrados
    Produto *lista = carregarOuAvisar(&qtd);           // lê as informações da loja
    if(lista == NULL) return;                         // interrompe se não houver arquivo disponível

    bubbleSort(lista, qtd, criterio, crescente);       // organiza a lista carregada sem salvar a ordenação
    printf("\n=== PRODUTOS ORDENADOS POR %s (%s) ===\n",
           criterio == 1 ? "NOME" : "PREÇO",
           criterio == 1 ? (crescente ? "A-Z" : "Z-A") : (crescente ? "menor -> maior" : "maior -> menor"));
    exibirTabela(lista, qtd);                          // exibe os resultados na sequência escolhida
    printf("(O arquivo %s não foi alterado.)\n", ARQUIVO);
    free(lista);                                       // limpa os dados carregados da memória
}

// Prepara campos de texto para o CSV, incluindo aspas quando necessário
void escreverCampoCSV(FILE *fp, const char *s){
    fputc('"', fp);                                    // inicia o campo de texto entre aspas
    for(; *s != '\0'; s++){                          // passa pelas letras do conteúdo
        if (*s == '"') fputc('"', fp);                 // duplica aspas que fazem parte do valor
        fputc(*s, fp);                                 // adiciona o caractere ao arquivo
    }
    fputc('"', fp);                                    // termina o campo de texto
}

// 12. EXPORTAR: salva os produtos em uma planilha CSV
void exportarCSV(void){
    int qtd;                                           // total de produtos registrados
    Produto *lista = carregarOuAvisar(&qtd);           // lê as informações da loja
    if(lista == NULL) return;                         // interrompe se não houver arquivo disponível

    FILE *fp = fopen(ARQUIVO_CSV, "w");                // abre o arquivo de exportação
    if(fp == NULL){                                  // verifica se a criação falhou
        printf("Erro ao criar o arquivo CSV!\n");
        free(lista);
        return;
    }
    fprintf(fp, "ID,Produto,Categoria,Preço (R$),Quantidade,Fornecedor\n"); // escreve os nomes das colunas
    for(int i = 0; i < qtd; i++){                    // salva cada produto separadamente
        const Produto *p = lista + i;                  // acessa o registro desta posição
        fprintf(fp, "%d,", p->id);                     // grava o código do produto
        escreverCampoCSV(fp, p->produto);   fputc(',', fp); // escreve o nome e separa o próximo campo
        escreverCampoCSV(fp, p->categoria); fputc(',', fp); // escreve a categoria antes do próximo valor
        fprintf(fp, "%.2f,%d,", p->preco, p->quantidade);   // exibe o preço junto das unidades em estoque
        escreverCampoCSV(fp, p->fornecedor);           // registra a empresa fornecedora
        fputc('\n', fp);                               // passa para o próximo produto
    }
    fclose(fp);                                        // encerra o acesso ao arquivo
    printf("%d produto(s) exportado(s) para %s\n", qtd, ARQUIVO_CSV);
    free(lista);                                       // limpa os dados carregados da memória
}
//  INÍCIO E MENU DO PROGRAMA

// Apresenta as funcionalidades disponíveis para o usuário
void exibirMenu(void){
    printf("\n========== CADASTRO DE LOJA ==========\n");
    printf(" 1 - Criar arquivo (dados iniciais)\n");
    printf(" 2 - Listar produtos\n");
    printf(" 3 - Inserir produto\n");
    printf(" 4 - Alterar produto\n");
    printf(" 5 - Excluir produto\n");
    printf(" 6 - Buscar por nome\n");
    printf(" 7 - Buscar por categoria\n");
    printf(" 8 - Ordenar por nome (A-Z)\n");
    printf(" 9 - Ordenar por nome (Z-A)\n");
    printf("10 - Ordenar por preço (menor -> maior)\n");
    printf("11 - Ordenar por preço (maior -> menor)\n");
    printf("12 - Exportar para CSV\n");
    printf(" 0 - Sair\n");
}

int main(){
    int opcao;                                         // guarda a escolha feita no menu

    do{                                               // mantém o sistema aberto até a saída
        exibirMenu();                                  // apresenta novamente o menu
        opcao = lerInteiro("Escolha: ", 0);            // recebe a escolha e verifica se é um inteiro

        switch(opcao){                               // direciona para a operação selecionada
            case 1:  criarArquivo();           break;  // inicia o cadastro com os produtos padrão
            case 2:  listarProdutos();         break;  // apresenta todos os produtos
            case 3:  inserirProduto();         break;  // cadastra um item novo
            case 4:  alterarProduto();         break;  // edita o produto selecionado
            case 5:  excluirProduto();         break;  // remove o produto informado
            case 6:  buscarProdutos(1);        break;  // pesquisa pela descrição
            case 7:  buscarProdutos(0);        break;  // pesquisa pelo grupo
            case 8:  ordenarProdutos(1, 1);    break;  // organiza os nomes em ordem alfabética
            case 9:  ordenarProdutos(1, 0);    break;  // organiza os nomes na ordem inversa
            case 10: ordenarProdutos(2, 1);    break;  // coloca os mais baratos primeiro
            case 11: ordenarProdutos(2, 0);    break;  // coloca os mais caros primeiro
            case 12: exportarCSV();            break;  // exporta os dados da loja
            case 0:  printf("Saindo...\n");    break;  // sai da repetição principal
            default: printf("Opção inválida!\n");      // avisa quando a opção não existe
        }
    } while(opcao != 0);                              // finaliza quando a escolha for zero
    return 0;                                          // informa que o programa terminou
}