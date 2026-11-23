#  Integração em C com Impressora Elgin i8 / i9
### Controle de Impressora Elgin via DLL `E1_Impressora01`
---

##  Sobre o Projeto

Este projeto foi desenvolvido por alunos com o objetivo de criar uma aplicação em **C** capaz de se comunicar com as impressoras térmicas **Elgin i8 e i9**, usando a biblioteca oficial **E1_Impressora01.dll** da Elgin.

O programa carrega a DLL dinamicamente (`LoadLibraryA` / `GetProcAddress`) e oferece um **menu no terminal** para configurar a conexão e testar as principais funções da impressora: impressão de texto, QR Code, código de barras, cupons **SAT** (venda e cancelamento), gaveta de dinheiro e sinal sonoro.

O trabalho faz parte da disciplina de **Construção de Algoritmos e Programação**.

---

##  Tecnologias Utilizadas

- **Linguagem C**
- **Windows API** (`windows.h`) para carregar a DLL
- **E1_Impressora01.dll** (SDK da Elgin)
- **Code::Blocks (MinGW)**, **MSYS2** ou outro compilador GCC para Windows

---

##  Estrutura do Projeto

```
/
├── main.c                  → Código-fonte principal
├── E1_Impressora01.dll     → DLL da Elgin (na raiz, mas NÃO versionada no Git; veja abaixo)
├── XMLSAT.xml              → XML de exemplo de venda SAT
├── CANC_SAT.xml            → XML de exemplo de cancelamento SAT
└── README.md               → Este arquivo
```

>  A DLL e os arquivos XML precisam ficar na **mesma pasta de onde o programa é executado**.
> A DLL é um arquivo grande (~20 MB) distribuído pela Elgin e por isso está no `.gitignore`: copie-a manualmente para a raiz do projeto (veja o passo 2 da instalação).

---

##  Instalação e Execução

###  1. Baixar o Projeto do GitHub
1. Acesse o repositório.
2. Clique no botão **Code**.
3. Selecione **Download ZIP**.
4. Aguarde o download e extraia o arquivo (botão direito → **Extrair aqui** / **Extract All**).

###  2. Preparar a DLL
A DLL precisa ter a **mesma arquitetura do compilador**:

| Compilador | DLL a usar |
|---|---|
| 32 bits (ex.: MinGW do Code::Blocks) | pasta **x86** |
| 64 bits (ex.: MSYS2 mingw64) | pasta **x64** |

Copie `E1_Impressora01.dll`, `XMLSAT.xml` e `CANC_SAT.xml` para a raiz do projeto.

###  3. Compilar

**Pelo terminal (GCC):**
```bash
gcc main.c -o impressora.exe
```

**Pelo Code::Blocks:**
1. Abra o `main.c` (ou crie um projeto de *Console application* em C com ele).
2. Clique em **Build and Run** (F9).

###  4. Executar
1. Conecte a impressora (USB, por exemplo) e ligue-a.
2. Execute `impressora.exe` a partir da pasta que contém a DLL.
3. Use o menu abaixo.

---

##  Menu do Programa

| Opção | Função |
|---|---|
| 1 | Configurar conexão (tipo, modelo, porta e parâmetro) |
| 2 | Abrir conexão |
| 3 | Fechar conexão |
| 4 | Imprimir texto (pergunta o texto e a quantidade de cópias) |
| 5 | Imprimir QR Code (pergunta o conteúdo) |
| 6 | Imprimir código de barras (CODE 128) |
| 7 | Imprimir XML SAT (`XMLSAT.xml`) |
| 8 | Imprimir XML de cancelamento SAT (`CANC_SAT.xml`) |
| 9 | Abrir gaveta (Elgin) |
| 10 | Abrir gaveta |
| 11 | Sinal sonoro (pergunta a quantidade de apitos) |
| 0 | Sair (fecha a conexão, se estiver aberta) |

**Login:** ao iniciar, o programa pede usuário e senha, com até **3 tentativas** e aviso das tentativas restantes (usuário `admin`, senha `1234`).

**Fluxo básico de uso:** login → `1` (configurar) → `2` (abrir conexão) → testes (`4` a `11`) → `0` (sair).

Todas as opções de impressão avisam e voltam ao menu, sem fazer perguntas, se a conexão estiver fechada.

Valores padrão: tipo `1` (USB), modelo `i9`, porta `USB`, parâmetro `0`.

---

##  Testes

- Login com senha correta, senha errada (3 tentativas) e bloqueio após a terceira falha
- Teste de conexão com a impressora (abrir e fechar)
- Impressão de texto simples
- Impressão de QR Code e código de barras
- Impressão de cupom SAT e de cancelamento SAT
- Abertura de gaveta e sinal sonoro
- Tratamento de erros: DLL ausente, arquivo XML não encontrado e comando sem conexão aberta

---

##  Problemas Comuns

| Problema | Causa provável |
|---|---|
| `Nao foi possivel carregar E1_Impressora01.dll` | DLL fora da raiz do projeto ou com arquitetura diferente do compilador (32/64 bits) |
| `Comando '...' nao encontrado na DLL` | Versão da DLL diferente da esperada |
| `Erro ao abrir conexao` | Impressora desligada, cabo desconectado ou modelo/porta incorretos |
| `nao foi possivel ler o arquivo ...xml` | O XML não está na pasta de onde o programa é executado |

---

##  Documentação Adicional

- Manual da Impressora **Elgin i9**
- Documentação do **SDK / DLL E1_Impressora01** da Elgin
- Referência **ESC/POS**

---

##  Autores  

- Carlos Eduardo
- José Pedro
- Kaique Leal