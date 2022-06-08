# SSC0142 - Redes de Computadores
# Trabalho de Arduino - Emissor Ímpar

## Objetivo

Este trabalho tem como objetivo a realização de uma comunicação síncrona entre duas plataformas Arduino. A mensagem deverá ser enviada de forma serial e síncrona.

## Membros

- **Giovanni Shibaki Camargo - 11796444**
- **Lucas Keiti Anbo Mihara - 11796472**
- **Vitor Caetano Brustolin - 11795589**

## Descrição do Projeto

Neste trabalho, o Arduino emissor receberá um caractere pela sua porta serial (através do Serial Monitor da plataforma) e deverá enviar de forma serial e síncrona ao Arduino receptor, que imprimirá os caracteres recebidos em sua porta serial, exibindo para o usuário através do Serial Monitor. Os caracteres deverão ser transmitidos de acordo com o código [ASCII](https://upload.wikimedia.org/wikipedia/commons/d/dd/ASCII-Table.svg).

## Requisitos

- A comunicação entre as duas plataformas deverá ser síncrona, ou seja, deve haver uma sincronização entre o emissor e o receptor através de um clock;

- A comunicação deverá implementar o controle de erros através de um bit de paridade;

- A comunicação deverá implementar um handshake simples entre o emissor e o receptor.

## Vídeo de Apresentação

O vídeo de apresentação do projeto se encontra neste [LINK](https://youtu.be/96DKPb0xCZk).

<iframe width="560" height="315" src="https://www.youtube.com/embed/96DKPb0xCZk" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>