![header](https://capsule-render.vercel.app/api?type=waving&color=33006e&fontColor=ffffff&height=200&section=header&text=Projeto%20TCP&fontSize=45&animation=fadeIn&fontAlignY=38)
## Sobre

Este trabalho foi proposto durante a disciplina de Redes de Computadores I, matéria ministrada ao longo do sexto período de Engenharia de Computação.

## Proposta

O projeto propôs implementar um sistema de envio de curtas mensagens em modelo multi-servidor. Desenvolvido em C, para Linux e sem o uso de bibliotecas especiais, a comunicação foi estabelecida via protocolo TCP/IP e o gerenciamento paralelo de conexões foi feito por meio da primitiva select.

## Objetivo

O objetivo foi o domínio da programação orientada a eventos se utilizando da primitiva select e da temporização por sinais.

## Orientações

Para execução do projeto, siga as seguintes orientações.

### 1. Configurando o ambiente

Instale o **WSL** caso esteja utilizando Windows, mas caso esteja usando uma distribuição Linux não há necessidade.

- [Link ensinando a instalar o WSL](https://learn.microsoft.com/pt-br/windows/wsl/install)

Via terminal, instale o **gcc** para a compilação dos arquivos e o **make** para o uso do makefile.

```
sudo apt install gcc && sudo apt install make
```
### 2. Clonando o repositório e compilando o código

Ainda no terminal e no diretório desejado, digite o seguinte comando:

```
git clone https://github.com/gabrielcecconello/trabalhoTCP.git
```

A pasta 'trabalhoTCP' será criada. Acesse-a e gere os binários com estes comados:

```
cd trabalhoTCP
```

```
make
```

Para caso queira limpar os binários, digite:

```
make clear
```
### 3. Executando o programa

Digite o seguinte para executar o servidor:

```
./server_exec
```

Abra um novo terminal e digite o seguinte para executar o cliente de envio de mensagens:

```
./client_exec {id_do_cliente_mensagem}
```

Abra um outro terminal e digite o seguinte para executar o cliente de exibição de mensagens:

```
./displayClient_exec {id_do_cliente_exibicao}
```
## Observações

Este projeto foi realizado puramente por razões didáticas e o código desenvolvido é extremamente simples, ou seja, **não** é seguro e **não** deve ser utilizado como base para uma aplicação robusta. Como foi divertido estabelecer na prática tal comunicação, resolveu-se compartilhar esse código como um marco de aprendizado. 

## Créditos

### Integrantes do grupo

- [Gabriel Cecconello](https://www.linkedin.com/in/gabriel-cecconello/)
- [Paula Talim](https://www.linkedin.com/in/paulatalim/)
- [Rafael Vicente](https://www.linkedin.com/in/rafael-vicente-8726a6204/)

### Professor Orientador

- [Ricardo Carlini](https://www.linkedin.com/in/carlinix/)

![footer](https://capsule-render.vercel.app/api?type=waving&color=33006e&height=200&section=footer&animation=fadeIn)
