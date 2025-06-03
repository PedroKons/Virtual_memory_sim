# Simulador de Memória Virtual

Este projeto implementa um simulador de memória virtual que gerencia páginas, TLB (Translation Lookaside Buffer) e memória física usando threads POSIX.

## Estrutura do Projeto

- `main.c`: Arquivo principal com a lógica de processamento de endereços virtuais
- `page_table.h/c`: Implementação da tabela de páginas
- `tlb.h/c`: Implementação do TLB (Translation Lookaside Buffer)
- `backing_store.txt`: Arquivo que simula o armazenamento secundário
- `data_memory.txt`: Arquivo que simula a memória física
- `addresses_16b.txt`: Arquivo com endereços virtuais para teste

## Requisitos

- GCC (GNU Compiler Collection)
- Sistema operacional Linux/Unix
- pthread (biblioteca POSIX Threads)

## Compilação

Para compilar o projeto, execute:

```bash
gcc -o virtual_memory_sim main.c page_table.c tlb.c -pthread
```

## Execução

Após a compilação, execute o programa:

```bash
./virtual_memory_sim
```

## Funcionalidades

- Simulação de memória virtual com paginação
- Gerenciamento de TLB com política LRU (Least Recently Used)
- Processamento concorrente de endereços usando threads
- Tratamento de page faults
- Leitura de dados da memória simulada

## Parâmetros do Sistema

- Tamanho da página: 256 bytes
- Tamanho da TLB: 16 entradas
- Tamanho da tabela de páginas: 32 entradas
- Número máximo de threads: 8

## Formato dos Arquivos

### addresses_16b.txt
Contém endereços virtuais de 16 bits em formato decimal ou hexadecimal (0x).

### backing_store.txt
Simula o armazenamento secundário com dados para cada página.

### data_memory.txt
Simula a memória física onde as páginas são carregadas.

## Saída

O programa exibe informações sobre:
- Endereços virtuais processados
- Hits e misses na TLB
- Page faults
- Valores lidos da memória
- Status de cada thread 