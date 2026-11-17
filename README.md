# 🛒 Sistema de Cadastro e Gerenciamento de Loja em C

Sistema de gerenciamento de produtos desenvolvido em linguagem C como parte de uma atividade acadêmica de Engenharia de Software.

O programa funciona pelo terminal e permite cadastrar, consultar, alterar, excluir e organizar produtos de uma loja, utilizando arquivos de texto para armazenar as informações.

## Funcionalidades

- Criação do arquivo de dados com quatro produtos iniciais
- Cadastro de novos produtos com ID automático
- Listagem dos produtos em formato de tabela
- Alteração das informações de produtos já cadastrados
- Exclusão de produtos pelo ID
- Pesquisa por nome ou categoria
- Ordenação alfabética crescente e decrescente
- Ordenação por preço, do menor para o maior e vice-versa
- Exportação dos dados para um arquivo CSV
- Validação de entradas e verificação de nomes duplicados

## Informações dos produtos

Cada produto possui os seguintes campos:

| Campo | Descrição |
|---|---|
| ID | Identificador numérico do produto |
| Produto | Nome do produto |
| Categoria | Categoria à qual pertence |
| Preço | Valor unitário em reais |
| Quantidade | Quantidade disponível em estoque |
| Fornecedor | Nome do fornecedor |

## Organização dos arquivos

O programa trabalha com três arquivos principais:

- **`loja.txt`** — armazena os produtos cadastrados e suas informações.
- **`temp.txt`** — utilizado temporariamente durante alterações e exclusões.
- **`loja.csv`** — gerado na exportação dos produtos para CSV.

Os campos do arquivo `loja.txt` são separados pelo caractere `|`.

Nas operações de alteração e exclusão, os dados atualizados são gravados em um arquivo temporário, que depois substitui o arquivo principal.

As ordenações são realizadas em memória e exibidas no terminal, sem modificar a ordem dos registros no arquivo original.

## Algoritmo de ordenação

Para organizar os produtos, utilizei o algoritmo **Bubble Sort**, que compara elementos vizinhos e troca suas posições quando estão fora da ordem desejada.

A ordenação pode ser realizada por nome ou preço, tanto em ordem crescente quanto decrescente.

## Tecnologias utilizadas

- Linguagem C
- Bibliotecas padrão da linguagem
- Manipulação de arquivos TXT e CSV

## Como executar

1. Instale um compilador C, como o GCC.
2. Baixe ou clone este repositório.
3. Abra o terminal na pasta do projeto.
4. Compile o arquivo:

```bash
gcc c-store-inventory-management.c -o c-store-inventory-management
```

5. Execute o programa:

**Windows:**

```bash
.\c-store-inventory-management.exe
```

**Linux/macOS:**

```bash
./c-store-inventory-management
```

Ao iniciar, o programa apresenta um menu com as operações disponíveis.

Na primeira utilização, selecione a opção **1 — Criar arquivo**, para gerar o arquivo com os produtos iniciais.

**Atenção:** utilizar essa opção novamente pode substituir os dados já cadastrados, mediante confirmação.

## Conceitos praticados

Durante o desenvolvimento, trabalhei principalmente com:

- Estruturas (`struct`)
- Ponteiros e passagem de parâmetros
- Alocação dinâmica de memória (`malloc` e `free`)
- Manipulação de arquivos (`fopen`, `fgets`, `fprintf`, `fclose`)
- Estruturas condicionais e de repetição
- Funções e organização do programa
- Manipulação e comparação de strings
- Algoritmos de ordenação
- Validação de entradas
- Leitura, gravação e exportação de dados

## Objetivo do projeto

Colocar em prática os conteúdos estudados em linguagem C, principalmente estruturas, ponteiros, algoritmos de ordenação e manipulação de arquivos.

A proposta foi desenvolver um programa capaz de reunir essas funcionalidades em uma aplicação de terminal, permitindo trabalhar com diferentes operações de cadastro e gerenciamento de informações.
