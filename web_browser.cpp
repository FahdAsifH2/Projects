#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class WebBrowser {
private:
    class Node {
    public:
        string url;
        Node* next;

        Node(const string& url) : url(url), next(nullptr) {}
    };

    Node* current;
    Node* undoStack;
    Node* redoStack;

    void push(Node*& stack, const string& url) {
        Node* newNode = new Node(url);
        newNode->next = stack;
        stack = newNode;
    }

    string pop(Node*& stack) {
        if (!stack) {
            return "";
        }
        string url = stack->url;
        Node* temp = stack;
        stack = stack->next;
        delete temp;
        return url;
    }

public:
    WebBrowser() : current(nullptr), undoStack(nullptr), redoStack(nullptr) {}

    ~WebBrowser() {
        clearStack(undoStack);
        clearStack(redoStack);
    }

    void openURL(const string& url) {
        ifstream file(url + ".txt");
        if (!file.is_open()) {
            cout << "Invalid URL: " << url << endl;
            return;
        }
        string line;
        cout << "Content of " << url << ".txt:" << endl;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
        push(undoStack, url);
        current = new Node(url);
    }

    void undo() {
        string prevURL = pop(undoStack);
        if (!prevURL.empty()) {
            push(redoStack, current->url);
            openURL(prevURL);
        } else {
            cout << "Nothing to undo." << endl;
        }
    }

    void redo() {
        string nextURL = pop(redoStack);
        if (!nextURL.empty()) {
            push(undoStack, current->url);
            openURL(nextURL);
        } else {
            cout << "Nothing to redo." << endl;
        }
    }

    void checkHistory(const string& url) {
        bool visited = false;
        Node* temp = undoStack;
        while (temp) {
            if (temp->url == url) {
                visited = true;
                break;
            }
            temp = temp->next;
        }
        if (visited) {
            cout << "You have visited " << url << endl;
        } else {
            cout << "You have not visited " << url << endl;
        }
    }

    void clearStack(Node*& stack) {
        while (stack) {
            Node* temp = stack;
            stack = stack->next;
            delete temp;
        }
    }
};

int main() {
    WebBrowser browser;

    while (true) {
        cout << "Enter command (URL/undo/redo/history): ";
        string command;
        getline(cin, command);

        if (command == "URL") {
            string url;
            cout << "Enter URL: ";
            getline(cin, url);
            browser.openURL(url);
        } else if (command == "undo") {
            browser.undo();
        } else if (command == "redo") {
            browser.redo();
        } else if (command == "history") {
            string url;
            cout << "Enter URL to check history: ";
            getline(cin, url);
            browser.checkHistory(url);
        } else {
            cout << "Invalid command" << endl;
        }
    }

return 0;
}
