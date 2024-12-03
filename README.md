# VIT - VOLUME INDICATOR AND TRANSMISSOR 

Medidor de volume com arduino mega e sensores de distância e de vazão 

---

**Projeto: Medição de Volume de um Recipiente de 2 Litros com Sensores e Arduino**  

Este projeto foi desenvolvido como parte de um trabalho acadêmico da Fundação Hermínio Ometto (FHO), com o objetivo principal de medir o volume de um recipiente de 2 litros. A proposta é utilizar dois sensores de distância, o ultrassônico HC-SR04 e o ToF laser VL53L0X, cujas leituras são combinadas para obter uma distância final mais precisa. Essa distância é utilizada, juntamente com os parâmetros de dimensões do recipiente, para calcular a porcentagem de enchimento.  

Além disso, o sistema conta com um medidor de vazão, o sensor YF-S201, instalado na entrada de líquido do recipiente. Esse sensor mede a vazão do líquido e, com base nesses dados, calcula uma segunda porcentagem de enchimento. Para garantir maior precisão na medição, as duas porcentagens são combinadas em uma média final. O resultado é exibido em uma tela OLED I2C 128x64, facilitando a visualização em tempo real.  

Adicionalmente, foi integrado ao sistema um buzzer que fornece alertas sonoros a cada 25% de enchimento do recipiente:  
- **25%**: 1 bip;  
- **50%**: 2 bips;  
- **75%**: 3 bips;  
- **100%**: 4 bips.  

Esse sistema integra conceitos de eletrônica, automação e programação com Arduino, demonstrando sua aplicabilidade em projetos de controle e monitoramento de líquidos.  

**Contato para dúvidas ou esclarecimentos**  
Caso tenha dúvidas ou necessite de informações adicionais sobre o projeto, entre em contato pelos e-mails:  
- **mjustino@alunos.fho.edu.br**  
- **emerson01@alunos.fho.edu.br**  

---  





