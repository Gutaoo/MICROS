import paho.mqtt.client as mqtt
import json


# Funcao retorno conexão entre cliente e broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectou ao broker")
    else:
        print("Falha ao conectar, código de retorno = " + rc)


# Funcao para formacão da resposta a ser enviada
def resposta_json(dicionario, seq, ar):
    dicionario.update({"seq": seq + 800000})
    dicionario.update({"nome": "Augusto Blauth Schneider"})
    dicionario.update({"turma": "4411"})
    del dicionario["tempExt"]                                   # Os elementos do dicionário python são atualizados
    del dicionario["tempInt"]                                   # Os elementos de temperatura e umidade são apagados
    del dicionario["umidade"]
    dicionario.update({"climatizado": ar})
    return json.dumps(dicionario)                               # A funcão retorna a resposta codificada em formato JSON


# Funcao para leitura das publicacões nos tópicos inscritos
def on_message(client, userdata, message):
    if message.topic == topic_data:                             # Se a mensagem recebida pertence ao tópico da chamada
        chamada = json.loads(message.payload.decode())          # Descarrega a mensagem em formato JSON para um dicionário python
        print(chamada)
        if chamada.get("matricula") == minha_matricula:         # Se minha matricula foi publicada
            print("Fui chamado")
            if chamada["tempInt"]["valor"] < chamada["tempExt"]["valor"]:
                ar = "Condicionador de ar ativo"
            else:
                ar = "Condicionador de ar inativo"
            resposta = resposta_json(chamada, chamada.get("seq"), ar)       # Chama a funcão que forma a resposta
            print("Enviando resposta: " + resposta)
            client.publish(topic_resposta, resposta)                        # Publica a resposta no tópico apropriado
    else:
        print(json.loads(message.payload.decode()))             # Exibe as mensagens recebidas nos outros tópicos


# Configurações MQTT
minha_matricula = 20000265
broker = "test.mosquitto.org"
port = 1883
topic_data = "Liberato/iotTro/44xx/data"
topic_resposta = "Liberato/iotTro/44xx/rply/" + str(minha_matricula)
topic_feedback = "Liberato/iotTro/44xx/ack"
topic_sucesso = "Liberato/iotTro/44xx/ack/" + str(minha_matricula)

# Cria um cliente MQTT e faz a conexão com o broker
client = mqtt.Client("Augusto")
client.connect(broker, port)
# Se inscreve nos tópicos de interesse
client.subscribe(topic_data)
client.subscribe(topic_feedback)
client.subscribe(topic_sucesso)
# Chamada das funcões
client.on_connect = on_connect
client.on_message = on_message

# Inicia e mantém a comunicação MQTT
client.loop_forever()
