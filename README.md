# Chat

## Alunos

### Henrique Martins de Messias - 17/0050394
### Guilherme de Lyra Pereira - 15/0128231

## Executando o Programa

No terminal, vá até a pasta do projeto e digite o seguinte comando para compilar:

```bash
    $ make
```

Para executar o código, digite:

```bash
    $ ./chat nickname # substitua 'nickname' por um nome de preferência
```

## Implementação do Protocolo
 - Os seguintes protocolos foram seguidos:
   - Usuário possuir fila com nome "/chat-nome" dentro do diretório "/dev/mqueue"
   - Enviar mensagem com "DE:PARA:MSG"
   - Receber mensagem com "DE: MSG"
   - Ao tentar enviar mensagem para usuário não existente, mostrar "UNKNOWNUSER PARA"
   - Ao tentar enviar mensagem por 3 vezes sem sucesso, mostrar "ERRO DE:PARA:MSG"
   - Ao usar comando 'list', listar usuários disponíveis
   - Impedir de fechar programa com CTRL C (usar comando 'sair' para fechar)
   - Se o destinatário for 'all', enviar mensagem para todos os usuários (impedir usuário de usar nome 'all')


## Problemas conhecidos
 - Envio de mensagens por broadcast apresenta algumas falhas

## Dificuldades de implementação do modelo de threads.
 - Não houve problemas na thread de recebimento de mensagens
 - Em relação a thread de envio de mensagens, houve dificuldade na implementação do envio para cada usuário no comando broadcast
 - Para enviar mensagem para um usuário único, a maior dificuldade foi com as tentivas de erro, mas a dificuldade não foi grande