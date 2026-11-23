#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

/*
 * Controle da impressora Elgin (i8 / i9) em C, usando a DLL E1_Impressora01.dll.
 *
 * A DLL precisa estar na RAIZ do projeto (na mesma pasta de onde o programa e executado),
 * junto com os arquivos XMLSAT.xml e CANC_SAT.xml.
 *
 * IMPORTANTE: a DLL precisa ter a mesma arquitetura do compilador:
 *   - compilador de 32 bits (ex.: MinGW do Code::Blocks)  -> use a DLL da pasta x86
 *   - compilador de 64 bits (ex.: MSYS2 mingw64)          -> use a DLL da pasta x64
 *
 * Fluxo do programa:
 *   1) Login (usuario e senha, ate 3 tentativas)
 *   2) Configurar Conexao  (opcao 1; obrigatorio antes de abrir; ENTER mantem o valor padrao)
 *   3) Abrir Conexao       (opcao 2; so funciona depois da configuracao)
 *   4) Comandos de impressao (opcoes 4 a 11; so funcionam com a conexao aberta)
 *   5) Fechar Conexao      (opcao 3) ou Sair (opcao 0, que tambem fecha a conexao)
 */

/* ===================== Ligacao com a DLL (E1_Impressora01.dll) ===================== */

/* Cada typedef descreve a "forma" de um comando da DLL: o que devolve e quais parametros recebe */
typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

/* Um ponteiro para cada comando. Depois de carregarDLL(), e so chamar: AbreConexaoImpressora(...) */
static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;

/* Procura um comando dentro da DLL. Devolve 1 se achou, 0 se nao achou. */
static int carregaFuncao(void *destino, const char *nome) {
    FARPROC f = GetProcAddress(dll, nome);
    if (f == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    *(FARPROC *)destino = f;
    return 1;
}

/* Carrega a DLL da raiz do projeto e todos os comandos. Devolve 1 se deu certo. */
static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira: a DLL esta na raiz do projeto? Ela tem a mesma arquitetura (32/64 bits) do compilador?\n");
        return 0;
    }

    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    return ok;
}

/* ===================== Variaveis do programa ===================== */

static int conexaoAberta = 0;       /* 0 = fechada, 1 = aberta */
static int conexaoConfigurada = 0;  /* 0 = ainda nao configurada, 1 = configurada */
static int tipo = 1;                /* 1 = USB, 2 = Serial, 3 = Rede... */
static char modelo[50] = "i9";      /* "i8" ou "i9" */
static char conexao[50] = "USB";
static int parametro = 0;

/* Credenciais do login simples (altere aqui para trocar usuario/senha) */
#define LOGIN_USUARIO   "admin"
#define LOGIN_SENHA     "1234"
#define LOGIN_TENTATIVAS 3

/* Tamanho maximo do texto digitado para impressao / QR Code */
#define TAM_TEXTO 256

/* Chave do QR Code usada no cancelamento SAT */
static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

/* ===================== Funcoes auxiliares ===================== */

/* Mostra a mensagem e le uma linha de texto digitada (sem o ENTER no final) */
static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    /* Se a linha foi maior que o buffer, descarta o resto para nao atrapalhar a proxima leitura */
    if (strchr(destino, '\n') == NULL && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

/* Mostra a mensagem e le um numero inteiro. Se digitar letras, pergunta de novo. */
static int lerInteiro(const char *mensagem) {
    char linha[50];
    char *fim;
    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        long valor = strtol(linha, &fim, 10);
        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }
        printf("Digite apenas numeros!\n");
    }
}

/* Como lerInteiro, mas se apenas ENTER for digitado devolve o valor padrao. */
static int lerInteiroPadrao(const char *mensagem, int padrao) {
    char linha[50];
    char *fim;
    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        if (linha[0] == '\0') {
            return padrao;
        }
        long valor = strtol(linha, &fim, 10);
        if (*fim == '\0') {
            return (int)valor;
        }
        printf("Digite apenas numeros!\n");
    }
}

/* Como capturarEntrada, mas se apenas ENTER for digitado mantem o valor que ja esta em destino. */
static void capturarEntradaPadrao(const char *mensagem, char *destino, int tamanho) {
    char linha[50];
    capturarEntrada(mensagem, linha, sizeof(linha));
    if (linha[0] != '\0') {
        strncpy(destino, linha, tamanho - 1);
        destino[tamanho - 1] = '\0';
    }
}

/* Le a senha sem mostrar o que foi digitado (aparece '*' no lugar de cada letra) */
static void lerSenha(const char *mensagem, char *destino, int tamanho) {
    int i = 0;
    int c;
    printf("%s", mensagem);
    while (1) {
        c = _getch();
        if (c == '\r' || c == '\n') {
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < tamanho - 1 && c >= 32) {
            destino[i++] = (char)c;
            printf("*");
        }
    }
    destino[i] = '\0';
    printf("\n");
}

/* Le um arquivo inteiro para a memoria. Devolve NULL se nao achar. Quem chama deve dar free(). */
static char *lerArquivo(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (arq == NULL) {
        return NULL;
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);

    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arq);
        return NULL;
    }
    size_t lidos = fread(conteudo, 1, tamanho, arq);
    conteudo[lidos] = '\0';
    fclose(arq);
    return conteudo;
}

/* Confere se a conexao esta aberta. Se nao estiver, avisa e devolve 0 (comando nao deve executar). */
static int verificaConexao(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada! Configure (1) e abra a conexao (2) antes de imprimir.\n");
        return 0;
    }
    return 1;
}

/* Mostra se o comando deu certo (retorno 0) ou o codigo de erro devolvido pela DLL */
static void mostraResultado(const char *acao, int retorno) {
    if (retorno == 0) {
        printf("%s: OK\n", acao);
    } else {
        printf("%s: erro (codigo %d)\n", acao, retorno);
    }
}

/* Avanca o papel e corta, conferindo o retorno de cada comando. */
static void avancarECortar(int linhas) {
    mostraResultado("Avanco de papel", AvancaPapel(linhas));
    mostraResultado("Corte", Corte(2));
}

/* ===================== Login ===================== */

/* Pede usuario e senha. Devolve 1 se logou, 0 se estourou as tentativas. */
static int fazerLogin(void) {
    char usuario[50];
    char senha[50];
    int tentativa;

    printf("*************************************************\n");
    printf("********************* LOGIN *********************\n");
    printf("*************************************************\n");

    for (tentativa = 1; tentativa <= LOGIN_TENTATIVAS; tentativa++) {
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        lerSenha("Senha: ", senha, sizeof(senha));

        if (strcmp(usuario, LOGIN_USUARIO) == 0 && strcmp(senha, LOGIN_SENHA) == 0) {
            printf("Login realizado com sucesso!\n");
            return 1;
        }
        printf("Usuario ou senha incorretos. Tentativas restantes: %d.\n\n", LOGIN_TENTATIVAS - tentativa);
    }
    return 0;
}

/* ===================== Funcoes da impressora ===================== */

void configurarConexao(void) {
    if (!conexaoAberta) {
        char msg[100];

        printf("Pressione ENTER para manter o valor padrao indicado entre colchetes.\n");

        sprintf(msg, "Tipo de conexao (1 = USB, 2 = Serial, etc.) [%d]: ", tipo);
        tipo = lerInteiroPadrao(msg, tipo);

        sprintf(msg, "Modelo da impressora (i8 ou i9) [%s]: ", modelo);
        capturarEntradaPadrao(msg, modelo, sizeof(modelo));

        sprintf(msg, "Porta de conexao (ex: USB) [%s]: ", conexao);
        capturarEntradaPadrao(msg, conexao, sizeof(conexao));

        sprintf(msg, "Parametro adicional (0 = padrao) [%d]: ", parametro);
        parametro = lerInteiroPadrao(msg, parametro);

        conexaoConfigurada = 1;
        printf("Parametros de conexao configurados com sucesso.\n");
    } else {
        printf("Conexao ja esta aberta. Feche a conexao para reconfigurar.\n");
    }
}

void abrirConexao(void) {
    if (conexaoAberta) {
        printf("Conexao ja esta aberta.\n");
        return;
    }
    if (!conexaoConfigurada) {
        printf("Configure a conexao (opcao 1) antes de abri-la.\n");
        return;
    }

    int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
    if (retorno == 0) {
        conexaoAberta = 1;
        printf("Conexao aberta com sucesso.\n");
    } else {
        printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
    }
}

void fecharConexao(void) {
    if (conexaoAberta) {
        int retorno = FechaConexaoImpressora();
        if (retorno == 0) {
            conexaoAberta = 0;
            printf("Conexao fechada com sucesso.\n");
        } else {
            printf("Erro ao fechar conexao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Nao ha conexao aberta para fechar.\n");
    }
}

void imprimirTexto(void) {
    if (!verificaConexao()) return;

    char texto[TAM_TEXTO];
    capturarEntrada("Digite o texto a ser impresso: ", texto, sizeof(texto));
    if (texto[0] == '\0') {
        printf("Nenhum texto digitado. Impressao cancelada.\n");
        return;
    }

    int copias = lerInteiro("Digite a quantidade de copias: ");
    if (copias < 1) {
        printf("Quantidade invalida. Impressao cancelada.\n");
        return;
    }

    int i;
    for (i = 1; i <= copias; i++) {
        /* posicao: 0 esquerda, 1 centro, 2 direita | estilo: 0 fonte A, 2 sublinhado, 8 negrito (somam) | tamanho: 0 normal */
        int retorno = ImpressaoTexto(texto, 1, 8, 0);

        /* ImpressaoTexto devolve a quantidade de dados enviados quando funciona: so e erro se for negativo */
        if (retorno < 0) {
            printf("Copia %d de %d - Impressao de texto: erro (codigo %d)\n", i, copias, retorno);
            break;  /* para na primeira falha */
        }
        printf("Copia %d de %d - Impressao de texto: OK\n", i, copias);
        avancarECortar(2);
    }
}

void imprimirQRCode(void) {
    if (!verificaConexao()) return;

    char texto[TAM_TEXTO];
    capturarEntrada("Digite o texto do QR Code: ", texto, sizeof(texto));
    if (texto[0] == '\0') {
        printf("Nenhum texto digitado. Impressao cancelada.\n");
        return;
    }

    /* tamanho do modulo: 6 | nivel de correcao: 4 (alto) */
    int retorno = ImpressaoQRCode(texto, 6, 4);
    mostraResultado("Impressao de QR Code", retorno);

    if (retorno == 0) {
        avancarECortar(5);
    }
}

void imprimirCodigoBarras(void) {
    if (!verificaConexao()) return;

    /* tipo 8 = CODE 128 | altura 100 | largura 2 | HRI 3 = numero acima e abaixo das barras */
    int retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    mostraResultado("Impressao de codigo de barras", retorno);

    if (retorno == 0) {
        avancarECortar(2);
    }
}

void imprimirXMLSAT(void) {
    if (!verificaConexao()) return;

    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Erro: nao foi possivel ler o arquivo XMLSAT.xml (ele esta na raiz do projeto?).\n");
        return;
    }

    int retorno = ImprimeXMLSAT(dadosXML, 0);
    mostraResultado("Impressao do XML SAT", retorno);

    if (retorno == 0) {
        avancarECortar(5);
    }

    free(dadosXML);
}

void imprimirXMLCancelamentoSAT(void) {
    if (!verificaConexao()) return;

    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Erro: nao foi possivel ler o arquivo CANC_SAT.xml (ele esta na raiz do projeto?).\n");
        return;
    }

    int retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    mostraResultado("Impressao do XML de cancelamento SAT", retorno);

    if (retorno == 0) {
        avancarECortar(5);
    }

    free(dadosXML);
}

void abrirGavetaElgin(void) {
    if (!verificaConexao()) return;

    /* pino 1 | tempo de inicio 50 | tempo de fim 50 */
    int retorno = AbreGavetaElgin(1, 50, 50);
    mostraResultado("Abertura da gaveta Elgin", retorno);
}

void abrirGaveta(void) {
    if (!verificaConexao()) return;

    /* pino 1 | tempo de inicio 5 | tempo de fim 10 */
    int retorno = AbreGaveta(1, 5, 10);
    mostraResultado("Abertura da gaveta", retorno);
}

void emitirSinalSonoro(void) {
    if (!verificaConexao()) return;

    int qtd = lerInteiro("Digite a quantidade de apitos: ");
    if (qtd < 1) {
        printf("Quantidade invalida. Sinal sonoro cancelado.\n");
        return;
    }

    /* qtd beeps | 5 = duracao do beep | 5 = intervalo entre beeps (unidades de 100 ms) */
    int retorno = SinalSonoro(qtd, 5, 5);
    mostraResultado("Sinal sonoro", retorno);
}

void exibirMenu(void) {
    printf("\n*************************************************\n");
    printf("**************** MENU IMPRESSORA ****************\n");
    printf("*************************************************\n");
    printf("Status: %s | %s\n\n",
           conexaoConfigurada ? "configurada" : "NAO configurada",
           conexaoAberta ? "conexao ABERTA" : "conexao fechada");

    printf("1  - Configurar Conexao\n");
    printf("2  - Abrir Conexao\n");
    printf("3  - Fechar Conexao\n");
    printf("4  - Impressao Texto\n");
    printf("5  - Impressao QRCode\n");
    printf("6  - Impressao Cod Barras\n");
    printf("7  - Impressao XML SAT\n");
    printf("8  - Impressao XML Canc SAT\n");
    printf("9  - Abrir Gaveta Elgin\n");
    printf("10 - Abrir Gaveta\n");
    printf("11 - Sinal Sonoro\n");
    printf("0  - Sair\n");
    printf("--------------------------------------\n");
}

/* ===================== Programa principal ===================== */

int main(void) {
    if (!fazerLogin()) {
        printf("Numero maximo de tentativas excedido. Programa encerrado.\n");
        return 1;
    }

    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        return 1;
    }

    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");

        switch (opcao) {
            case 1:  configurarConexao();         break;
            case 2:  abrirConexao();              break;
            case 3:  fecharConexao();             break;
            case 4:  imprimirTexto();             break;
            case 5:  imprimirQRCode();            break;
            case 6:  imprimirCodigoBarras();      break;
            case 7:  imprimirXMLSAT();            break;
            case 8:  imprimirXMLCancelamentoSAT();break;
            case 9:  abrirGavetaElgin();          break;
            case 10: abrirGaveta();               break;
            case 11: emitirSinalSonoro();         break;
            case 0:
                if (conexaoAberta) {
                    fecharConexao();
                }
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    }

    FreeLibrary(dll);
    return 0;
}