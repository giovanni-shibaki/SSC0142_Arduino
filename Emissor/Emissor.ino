#define PINO_RX 13
#define PINO_TX 13
#define PINO_RTS 12
#define PINO_CTS 11
#define BAUD_RATE 1
#define HALF_BAUD 1 / (2 * BAUD_RATE)

void configuraTemporizador(int baud_rate)
{
    int frequencia;
    frequencia = constrain(baud_rate, 1, 1500);
    // set timer1 interrupt
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1 = 0;  // initialize counter value to 0
    // OCR1A = contador;// = (16*10^6) / (10*1024) - 1 (must be <65536)
    OCR1A = ((16 * pow(10, 6)) / (1024 * frequencia)) - 1;
    // turn on CTC mode (clear time on compare)
    TCCR1B |= (1 << WGM12);
    // Turn T1 off
    TCCR1B &= 0xF8;
    // Disable interrupts
    TIMSK1 = 0;
    TIFR1 = 0;
}

void iniciaTemporizador()
{
    Serial.println("T1 iniciado");
    TCNT1 = 0; // initialize counter value to 0
    TIFR1 = 0;
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

void paraTemporizador()
{
    Serial.println("T1 parado");
    // Turn T1 off
    TCCR1B &= 0xF8;
    TIMSK1 = 0;
}

enum ESTADOS
{
    INICIO = 0,
    DADOS,
    PARIDADE,
    FIM
};

String msg = "";
volatile int estadoAtual = 0;
volatile char charAtual = 0;
volatile int indiceBitAtual = 0;
volatile int contaParidade = 0;
volatile int contadorFim = 0;
volatile boolean transmitindo = false;

// Rotina de interrupcao do timer1
ISR(TIMER1_COMPA_vect)
{
    enviaSequenciaChar();
}

void enviaSequenciaChar()
{
    switch (estadoAtual)
    {
        case INICIO:
        {
            iniciaTemporizador();
            Serial.print("char atual: ");
            Serial.print(charAtual);
            Serial.println("");
            // Enviar caractere de inicio
            transmitindo = true;
            digitalWrite(PINO_TX, LOW);
            Serial.println("0");
            estadoAtual++;
            return;
        }
        case DADOS:
        {
            // Leitura do bit na posicao do indice
            boolean singleBit = bitRead(charAtual, indiceBitAtual);
            if (singleBit)
            {
                contaParidade++;
            }

            // Envio do bit
            digitalWrite(PINO_TX, singleBit);
            Serial.print(singleBit);
            Serial.println("");

            indiceBitAtual++;

            // Caso todos os bits de dados tenham sido enviados, ir para o estado PARIDADE para o envio do bit de paridade
            if (indiceBitAtual == 7)
                estadoAtual++;
            return;
        }
        case PARIDADE:
        {
            // Como nosso grupo eh de paridade impar
            // Se houver um numero impar de bits 1 no dado, o bit de paridade sera 0
            // Se houver um numero par de bits 1 no dado, o bit de paridade sera 1
            digitalWrite(PINO_TX, !(contaParidade % 2));
            Serial.print(contaParidade%2);
            Serial.println("");
            // Vai para o estado FIM para envio dos 2 bits de flag de fim
            estadoAtual++;
            return;
        }
        case FIM:
        {
            // Transmitir os 2 bits de flag de fim
            if (contadorFim++ < 2)
            {
                digitalWrite(PINO_TX, HIGH);
                Serial.println("1");
                return;
            }
            contadorFim = 0;
            indiceBitAtual = 0;
            estadoAtual = 0;
            contaParidade = 0;
            transmitindo = false;
            return;
        }
    }
}

// Executada uma vez quando o Arduino reseta
void setup()
{
    // desabilita interrupcoes
    noInterrupts();
    // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
    Serial.begin(9600);
    // Inicializa TX ou RX
    pinMode(PINO_TX, OUTPUT);
    pinMode(PINO_RTS, OUTPUT);
    pinMode(PINO_CTS, INPUT);
    // Configura timer
    configuraTemporizador(BAUD_RATE);
    // habilita interrupcoes
    interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop()
{
    // Passo a passo
    // O Emissor espera um novo dado no serial monitor
    // Para comecar a transmitir deve fazer o handshake:
    // Emissor seta RTS para 1
    // Receptor ve que RTS eh 1 e seta o CTS para 1 caso esteja pronto para receber
    // Emissor ve que CTS eh 1 e inicia a transmissao com o clock
    // A cada transicao de clock (de 0 para 1 ou de 1 para 0) o receptor ve o dado que chegou no RX
    // Quando termina a transmissao, o Emissor seta o RTS para 0
    // O receptor entao checa o bit de paridade, se bater com o que foi recebido, imprime o resultado na tela
    // O receptor seta o CTS para 0

    // O Emissor espera um novo dado no monitor serial
    Serial.println("Digite alguma coisa:");
    while (Serial.available() == 0){
    }

    // Leitura da string a ser enviada atraves do monitor serial
    msg = Serial.readString();
    Serial.println(msg);

    // ========================================================================================================
    // Calcula paridade
    for (int i = 0; i < msg.length()-1; i++)
    {
        charAtual = msg.charAt(i);
        // =========================================================================================================
        // HANDSHAKE

        // Emissor seta RTS para 1
        digitalWrite(PINO_RTS, HIGH);

        // Aguardar o receptor estar pronto para receber
        while (digitalRead(PINO_CTS) == HIGH) {
        } // conclui o handshake (quando o receptor setar o pino CTS para 1)

        enviaSequenciaChar();

        // Para o loop principal do arduino enquanto a msg esta sendo transmitida
        while (transmitindo){
        }
        
        paraTemporizador();

        // Transmissao finalizada Emissor seta o RTS para LOW
        digitalWrite(PINO_RTS, LOW);
    }
    Serial.println("Mensagem enviada!\n");
}
