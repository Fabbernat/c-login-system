#include <stdio.h>

int main(int argc, char *argv[]) {
    FILE *file = fopen("index.html", "w");

    if (file == NULL) {
        printf("Error: Could not create file.\n");
        return 1;
    }

    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html lang=\"en\">\n");
    fprintf(file, "<head>\n");
    fprintf(file, "    <meta charset=\"UTF-8\">\n");
    fprintf(file, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(file, "    <title>Login/Signup</title>\n");
    fprintf(file, "    <style>\n");
    fprintf(file, "        body {\n");
    fprintf(file, "            font-family: sans-serif;\n");
    fprintf(file, "            display: flex;\n");
    fprintf(file, "            justify-content: center;\n");
    fprintf(file, "            align-items: center;\n");
    fprintf(file, "            height: 100vh;\n");
    fprintf(file, "            margin: 0;\n");
    fprintf(file, "        }\n");
    fprintf(file, "        .container {\n");
    fprintf(file, "            border: 1px solid #ccc;\n");
    fprintf(file, "            padding: 20px;\n");
    fprintf(file, "            border-radius: 5px;\n");
    fprintf(file, "        }\n");
    fprintf(file, "        input[type=\"text\"], input[type=\"password\"] {\n");
    fprintf(file, "            width: 100%;\n");
    fprintf(file, "            padding: 10px;\n");
    fprintf(file, "            margin: 8px 0;\n");
    fprintf(file, "            box-sizing: border-box;\n");
    fprintf(file, "        }\n");
    fprintf(file, "        button {\n");
    fprintf(file, "            background-color: #4CAF50;\n");
    fprintf(file, "            color: white;\n");
    fprintf(file, "            padding: 10px 15px;\n");
    fprintf(file, "            border: none;\n");
    fprintf(file, "            border-radius: 4px;\n");
    fprintf(file, "            cursor: pointer;\n");
    fprintf(file, "        }\n");
    fprintf(file, "    </style>\n");
    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");
    fprintf(file, "    <div class=\"container\">\n");
    fprintf(file, "        <h2>Log In</h2>\n");
    fprintf(file, "        <form>\n");
    fprintf(file, "            <input type=\"text\" placeholder=\"Username\" name=\"username\">\n");
    fprintf(file, "            <input type=\"password\" placeholder=\"Password\" name=\"password\">\n");
    fprintf(file, "            <button type=\"submit\">Log In</button>\n");
    fprintf(file, "        </form>\n");
    fprintf(file, "        <hr>\n");
    fprintf(file, "        <h2>Sign Up</h2>\n");
    fprintf(file, "        <form>\n");
    fprintf(file, "            <input type=\"text\" placeholder=\"Username\" name=\"new_username\">\n");
    fprintf(file, "            <input type=\"password\" placeholder=\"Password\" name=\"new_password\">\n");
    fprintf(file, "            <button type=\"submit\">Sign Up</button>\n");
    fprintf(file, "        </form>\n");
    fprintf(file, "    </div>\n");
    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");

    fclose(file);
    return 0;
}