
# ProcessFlow

Orquestrador de processos que registra tarefas (programas do sistema) e as executa por meio de processos filhos, usando `fork()`, `exec()`, `wait()`/`waitpid()`, `pipe()` e `dup2()`. Suporta execução sequencial, paralela, em pipe, redirecionamento de entrada/saída, mudança de diretório de trabalho e execução em segundo plano com controle de jobs.

## Sistema operacional utilizado

Implementado e testado em **Windows 11 com WSL (Ubuntu)**. Por usar apenas chamadas de sistema POSIX padrão (`fork`, `execvp`, `waitpid`, `pipe`, `dup2`, `open`, `chdir`), o programa também deve compilar e rodar normalmente em Linux nativo e macOS, sem alterações no código.

## Arquivos utilizados

```
src/
├── main.c              -> função main(); loop do modo interativo e do modo workflow (.pf)
├── task.c / task.h     -> struct Task e struct TaskRegistry; cadastro e busca de tarefas
├── job.c / job.h       -> struct Job e struct JobRegistry; controle de jobs em background
├── acoesprocess.c/.h   -> fork/exec/wait, pipe, redirecionamento de I/O e workdir
├── basic.pf            -> arquivo de exemplo no modo workflow, usado como teste manual
├── Makefile             -> compilação, limpeza e execução de testes
└── README.md            -> este arquivo
```

## Como compilar

Dentro da pasta `src/`:

```bash
make
```
ou 
```bash
   gcc -Wall -Wextra -std=c11 -g -o processflow main.c task.c job.c acoesprocess.c

```

Isso gera o executável `processflow` na própria pasta `src/`, a partir de `main.c`, `task.c`, `job.c` e `acoesprocess.c`.

Para limpar os arquivos compilados (`.o` e o executável):

```bash
make clean
```

## Como executar

**Modo interativo** (sem argumentos), apresenta o prompt `processflow>`:

```bash
./processflow
```

**Modo workflow**, lendo comandos de um arquivo `.pf`:

```bash
./processflow basic.pf
```

Em ambos os modos, o programa é encerrado com o comando `exit` (ou `CTRL-D` no modo interativo).

## Como testar

Um arquivo de exemplo (`basic.pf`) já está incluído, cobrindo cadastro de tarefas, execução sequencial/paralela, pipe, redirecionamento e background:

```bash
./processflow basic.pf
```

Também é possível rodar os testes automatizados definidos no Makefile:

```bash
make test
```

Para testes manuais adicionais, recomenda-se criar outros arquivos `.pf` propositalmente com erros (tarefa não cadastrada, programa inexistente, arquivo de entrada/saída inválido, diretório inexistente em `workdir`, arquivo sem o comando `exit`) para verificar se o ProcessFlow trata cada situação de forma coerente, sem encerrar de forma inesperada.
