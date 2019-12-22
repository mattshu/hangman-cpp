/*
Hangman Game -- Completed 12-21-2019 19:06est by Matt Kelley
Word source: https://github.com/Xethron/Hangman/blob/master/words.txt

Program Blueprint
-------------------

    . Classical game of Hangman. Program starts by asking Player
       which difficulty level they want.

    . Console-based game which draws the full hangman post like this:
            ________________
            ||             :
            ||             :_           
            ||            (  )
            ||             ||   
            ||          \__||__/        
            ||             ||
            ||             ||
            ||          __/  \__
            ||
            ||         _ A R _ _
        ____||__________________________
        | Guesses:                     |
        |CDGQZSW_______________________|
        Enter your guess: 
        

        The word goes below the hangee, with incorrect guesses below it.

    1. Choose difficulty Easy, Medium, Hard, Impossible
        Easy:       3-4 length words
        Medium:     5-7 length words
        Hard:       7+ length words
        Impossible: Random gibberish words

    2. Player gets seven tries:
        Try 1:  Show Head
        Try 2:  Show Neck
        Try 3:  Show Left Arm
        Try 4:  Show Right Arm
        Try 5:  Show Torso
        Try 6:  Show Left Leg
        Try 7:  Show Right Leg (fail)

        Try 0
        Game Loop
        Player guesses letter
        If input not letter
            Repeat loop
        If guess is in the secret word,
            show all matches
            If word is revealed, win game
        If guess is not in word, 
            If try is 7, fail game
            Else add letter to bottom, 
                 add limb to body
        
        Increment try
        End Game Loop

*/

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

class WordHandler
{
    private:
        std::vector<std::string> easyWords{};
        std::vector<std::string> mediumWords{};
        std::vector<std::string> hardWords{};
        std::vector<std::vector<std::string>> library
            { easyWords, mediumWords, hardWords };
            
        void loadWords();
    
    public:
        WordHandler()
        {
            srand(time(NULL));  // seed rand() for impossible difficulty
            loadWords();
        }

        std::string getWord(int difficulty);
};

void WordHandler::loadWords()
{ 
    std::vector<std::string> wordFiles{"easy_words.txt","medium_words.txt","hard_words.txt"};
    auto rng = std::default_random_engine{};
    for (int difficulty = 0; difficulty < 3; difficulty++)
    {
        std::vector<std::string> words{};
        std::string path{wordFiles[difficulty]};
        std::ifstream reader{path};
        if (reader.is_open())
        {
            std::string word{};
            while (std::getline(reader, word))
            {  // Uppercase characters
                std::transform(word.begin(), word.end(), word.begin(), toupper);
                words.push_back(word);
            }
            
            // Shuffle the words
            std::shuffle(words.begin(), words.end(), std::random_device{});

            library[difficulty] = words;
        }
        else
            std::cout << "Error reading word files into library!\n";
    }  
}

std::string WordHandler::getWord(int difficulty)
{
    if (difficulty > 2) 
    {  // Impossible difficulty
        const int length{ 5 };
        std::string word{};
        std::string alpha{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
        for (int i = 0; i < length; i++)
            word = word + alpha[rand() % 26];
        return word;
    }

    static int index{};
    std::vector<std::string> words = library[difficulty];
    return words[index++];

}  

int pickDifficulty()
{
    int difficulty{};
    while (difficulty < 1 || difficulty > 4)
    {
        std::cout << "Choose difficulty:\n";
        std::cout << "(1 = Easy, 2 = Medium, 3 = Hard, 4 = Impossible)\n";
        std::cout << "Enter choice: ";
        int input{};
        std::cin >> input;  // TODO: validate
        if (std::cin.fail())
        {
            // Reset cin upon invalid input
            std::cin.clear();
            std::cin.ignore();
        }
        else
            difficulty = input;
    }
    return difficulty - 1;

}

void drawHangingMan(int strikes)
{
    std::string postTopSection{"\t________________\n"
                               "\t||             :\n"
                               "\t||             :\n"};
    std::string head{(strikes >= 1) ? "            (  )" : ""};
    std::string postHeadSection{"\t||" + head};
    std::string neck{(strikes >= 2) ? "             ||" : ""};
    std::string postNeckSection{"\t||" + neck};
    std::string leftArm{(strikes >= 3) ? "          \\__||" : "   "};
    std::string rightArm{(strikes >= 4) ? "__/" : ""};
    std::string postArmSection{"\t||" + leftArm + rightArm};
    std::string torso{(strikes >= 5) ? "             ||" : ""};
    std::string postTorsoSections{"\t||" + torso + '\n' + "\t||" + torso};
    std::string leftFoot{(strikes >= 6) ? "          __/" : ""};
    std::string rightFoot{(strikes >= 7) ? "  \\__" : ""};
    std::string postFeetSection{"\t||" + leftFoot + rightFoot};

    std::cout << postTopSection
                 + postHeadSection + '\n'
                 + postNeckSection + '\n'
                 + postArmSection + '\n'
                 + postTorsoSections + '\n'
                 + postFeetSection + '\n';
}


int main()
{
    std::cout << "Hangman ver.?\n";
    int strikes{};
    int difficulty{ pickDifficulty() };
    WordHandler *wordHandler = new WordHandler{};
    std::string secretWord = wordHandler->getWord(difficulty);
    std::string guesses{};

    // Placeholders (underscores) to hide the word
    std::string formattedWord{'_'};
    for (int i = 0; i < secretWord.length() - 1; i++)
        formattedWord = formattedWord + " _";

    // Center the secret word underneath the hangman using a margin
    std::string marginLeft(14 - formattedWord.length()
                              + (formattedWord.length() / 2), ' ');
    
    // Begin game loop
    while (true)
    {
        drawHangingMan(strikes);
        std::cout << "\t||\n\t||" << marginLeft << formattedWord << "\n\t||\n";
        std::cout << "________||______________________\n";
        std::cout << "| Guesses:                     |\n";
        std::string bottomRightPost{ std::string(30 - guesses.size(), '_') };
        std::cout << "|" << guesses << bottomRightPost << "|\n";
        std::cout << "Enter guess: ";

        if (strikes >= 7)  
        {  // Fail game
            std::cout << "\nYou lost! The word was: " << secretWord << '\n';
            return 0;  // Exit
        }
        else if (formattedWord.find('_') == std::string::npos)
        {  // Win game
            std::cout << "\nYou've won!\n";
            return 0;  // Exit
        }
        
        std::string inputStr{};
        std::cin >> inputStr;
        
        char input{inputStr[0]};

        if (!std::isalpha(input))
            continue;

        input = std::toupper(input);

        // Check if input is already in guesses
        if (guesses.find(input) != std::string::npos)
            continue;

        size_t pos = secretWord.find(input);
        
        if (pos == std::string::npos) 
        {  // Nothing found, add input to guesses and add strike
            guesses = guesses + input;
            strikes++;
        }

        while (pos != std::string::npos)
        {  // Found input, display hidden letters
            formattedWord[pos * 2] = input;

            // Detect if char is repeated
            pos = secretWord.find(input, pos + 1);
        }
    }
    return 0;
}