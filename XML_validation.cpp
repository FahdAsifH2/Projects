#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class XML 
{
public:
    string tag;
    int lineNumber;
    bool isOpeningTag;

    XML() : lineNumber(0), isOpeningTag(false) {}

    XML(const string& tag, int lineNumber, bool isOpeningTag)
        : tag(tag), lineNumber(lineNumber), isOpeningTag(isOpeningTag) {}
};

template <class T>
class Stack {
private:

    class Node 
    {
    public:
        T data;
        Node* next;

        Node(const T& data) : data(data), next(nullptr) {}
    };

    Node* top;

public:
    Stack() : top(nullptr) {}

    bool IsEmpty() const {
        return top == nullptr;
    }

    void push(const T& val) {
        Node* newnode = new Node(val);
        newnode->next = top;
        top = newnode;
    }

    bool pop() {
        if (IsEmpty()) {
            return false;
        }
        Node* temp = top;
        top = top->next;
        delete temp;
        return true;
    }

    bool Top(T& val) const {
        if (IsEmpty()) {
            return false;
        }
        val = top->data;
        return true;
    }

    ~Stack() {
        while (!IsEmpty()) {
            pop();
        }
    }
};

bool readXMLFile(const string& filename, Stack<XML>& stk) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file: " << filename << endl;
        return false;
    }

    string line;
    int lineNumber = 1;
    bool isXmlDeclarationProcessed = false;

    while (getline(file, line)) {
        if (!isXmlDeclarationProcessed && line.find("<?xml") == 0) {
            // Skip XML declaration line
            lineNumber++;
            continue;
        }

        int len = line.length();
        int i = 0;

        while (i < len) {
            char ch = line[i];

            if (ch == '<') {
                int startPos = i + 1;
                bool isOpeningTag = true;

                if (line[startPos] == '/') {
                    isOpeningTag = false;
                    startPos++; // Skip the '/'
                }

                // Find the end position of the tag
                while (i < len && line[i] != '>') i++;

                if (i >= len) {
                    cout << "Error: Invalid XML tag at line " << lineNumber << endl;
                    return false;
                }

                int endPos = i;
                string tagName = line.substr(startPos, endPos - startPos);

                // If it's an opening tag, push it onto the stack
                if (isOpeningTag) {
                    XML xmlElement(tagName, lineNumber, true);
                    stk.push(xmlElement);
                } else { // If it's a closing tag, check if it matches the top of the stack
                    XML topElement;
                    if (!stk.Top(topElement)) {
                        cout << "Error: Mismatched closing tag '" << tagName << "' at line " << lineNumber << endl;
                        return false;
                    }

                    if (topElement.tag != tagName) {
                        cout << "Error: Mismatched closing tag '" << tagName << "' at line " << lineNumber << endl;
                        return false;
                    }

                    stk.pop();
                }
            }

            i++; // Move to the next character
        }

        lineNumber++;
    }

    file.close();

    // Check if there are any unclosed tags left in the stack
    if (!stk.IsEmpty()) {
        XML topElement;
        stk.Top(topElement);
        cout << "Error: Mismatched opening tag '" << topElement.tag << "' (no corresponding closing tag)" << endl;
        return false;
    }

    return true;
}

void validateXML(Stack<XML>& stk) {
    Stack<string> tagStack;

    while (!stk.IsEmpty()) {
        XML xmlElement;
        stk.Top(xmlElement);
        stk.pop();

        if (xmlElement.isOpeningTag) {
            tagStack.push(xmlElement.tag);
        } else {
            string topTag;
            if (!tagStack.Top(topTag)) {
                throw runtime_error("Mismatched closing tag '" + xmlElement.tag + "' at line " + to_string(xmlElement.lineNumber));
            } else if (topTag != xmlElement.tag) {
                throw runtime_error("Mismatched closing tag '" + xmlElement.tag + "' at line " + to_string(xmlElement.lineNumber));
            } else {
                tagStack.pop();
            }
        }
    }

    while (!tagStack.IsEmpty()) {
        string topTag;
        tagStack.Top(topTag);
        tagStack.pop();
        throw runtime_error("Mismatched opening tag '" + topTag + "' (no corresponding closing tag)");
    }
}

int main() {
    string filename = "input.xml";
    Stack<XML> stk;

    try {
        if (readXMLFile(filename, stk)) {
            cout << "Successfully read XML file." << endl;
            validateXML(stk);
            cout << "XML validation successful." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1; // indicate error to the caller
    }

    return 0;
}
