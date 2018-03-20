# Led-Cube

Modo proporcional: variando-se a tensão de saída do potenciômetro de 0 a 100%, varia-se a quantidade de LEDs acesa no cubo, utilizando-se os primeiros 9 LEDs dispostos na primeira face do cubo. 

Modo binário: Os 10 primeiros LEDs representaram o valor binário obtido pela digitalização do sinal do potenciômetro – caso o valor digitalizado seja 0, todos os 10 LEDs devem ficar apagados; caso seja 1, apenas o LED0 ficaria aceso e assim sucessivamente.

Modo monitor simples: monitora-se apenas a temperatura interna do RN. O cubo de LEDs funcionam como indicador visual da temperatura do RN seguindo-se a seguinte estratégia: 

Condição                        Faixa de temperatura                Efeito visual 
Normal                              36,5 a 37 °C                  LEDs verdes acesos 
Anormalidade moderada         35 a 36,4 °C ou 37 a 38 °C         LEDs azuis piscando 
Anormalidade severa              <35 °C ou > 37,5 °C            LEDs vermelhos piscando 

Modo monitor completo: faz tudo o que o modo monitor simples executa e também monitora a temperatura ambiente. Caso a diferença entre a temperatura ambiente e a temperatura do RN for menor que 8 °C ou maior que 18 °C (ambiente de temperatura controlada), todos os 27 LEDs piscam indicando que o ambiente está se tornando insalubre ao RN.
