void TRAIN() {

    double N[hidden_neurons], D[output_neurons][hidden_neurons], Dk[output_neurons], Y[output_neurons];
    double ERR[output_neurons], ALFA = 0.02;

    for (int i = 0; i < hidden_neurons; i++)
    {
      for (int j = 0; j < inputs__neurons; j++)
        {
            Wnn[j][i] = (rand() % 10) - (rand() % 10);
        }
        for (int j = 0; j < output_neurons; j++)
        {
            Wss[j][i] = (rand() % 10) - (rand() % 10);
        }

    }
    
    for (int i = 0; i < epoch; i++) {

        for (int i = 0; i < train_data_per_input; i++) { // позиция входного значения

            for (int m = 0; m < hidden_neurons; m++) { // количество нейронов
                N[m] = 0.0;
                for (int n = 0; n < inputs__neurons; n++) { // количество входых нейронов
                    N[m] += Wnn[n][m] * Xnn[i][n];
                }
                N[m] = 1 / (1 + exp(-N[m]));
            }

            for (int n = 0; n < output_neurons; n++) {
                Y[n] = 0.0;
                for (int m = 0; m < hidden_neurons; m++) {
                    Y[n] += N[m] * Wss[n][m];
                }
                Y[n] = 1 / (1 + exp(-Y[n]));
            }

            // ERROR 
            for (int n = 0; n < output_neurons; n++) {
                ERR[n] = 0.0;
                ERR[n] = YD[i][n] - Y[n];
                //Обратное распростонение 
                Dk[n] = Y[n] * (1 - Y[n]) * ERR[n];
            }

            // новые веса выходных нейронов
            for (int n = 0; n < output_neurons; n++) {

                for (int m = 0; m < hidden_neurons; m++) {
                    Wss[n][m] = Wss[n][m] + ALFA * N[m] * Dk[n];

                }

            }

            for (int n = 0; n < output_neurons; n++) {
                for (int m = 0; m < hidden_neurons; m++) {
                    D[n][m] = 0.0;
                    D[n][m] = N[m] * (1 - N[m]) * Wss[n][m] * Dk[n];
                }
            }
            for (int x = 0; x < output_neurons; x++) {
                for (int m = 0; m < hidden_neurons; m++) {
                    for (int n = 0; n < inputs__neurons; n++) {
                        Wnn[n][m] = Wnn[n][m] + ALFA * Xnn[n][i] * D[x][m];
                    }
                }
            }

        }
        
    }
}