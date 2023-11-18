O objetivo do trabalho era utilizar o protocolo MQTT para responder uma chamada.

No MQTT os clientes (meu computador no projeto) se conectam a um broker (microcontrolador com TCP/IP) e se inscrevem em tópicos de interrese. No projeto, a chamada de vários alunos estava constantemente sendo realizada
pelo tópico data, quando sou chamado eu publico uma resposta em outro tópico e o microcontrolador envia um feedback da resposta. O projeto foi relizado em Python.
