O objetivo do trabalho era utilizar o protocolo MQTT para responder uma chamada.

No MQTT os clientes (meu computador no projeto) se conectam a um broker (microcontrolador com acesso à internet, usando o protocolo TCP/IP) e se inscrevem em tópicos de interrese. No projeto, a chamada de vários alunos estava constantemente sendo realizada pelo tópico data, quando o programa ve que sou chamado, ele publica uma resposta no tópico de resposta, e o microcontrolador envia um feedback da resposta no tópico de feedback. O projeto foi relizado em Python.
