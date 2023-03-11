#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

#ifdef _WIN32
    const char pathSep = '\\';
#else
    const char pathSep = '/';
#endif

const std::string
    usageMsg = "binembed <namespace> <resource-path>",
    indentation = "    ",
    hexAlphabet = "0123456789abcdef";
const char bytesPerLine = 16;
const std::regex identifierValidator("^[A-Za-z_]\\w*(::[A-Za-z_]\\w*)*$");

void usage(std::string errMsg)
{
    std::cerr
        << errMsg << std::endl
        << std::endl
        << usageMsg << std::endl;
}

// Performs basic sanity checking on namespace identifier. Does not check for
// keword collision.
bool validateNamespace(std::string ns)
{
    return std::regex_match(ns, identifierValidator);
}

std::string fnameFromPath(std::string path)
{
    std::string fname = path;

    int lastSep = path.find_last_of(pathSep);
    if (lastSep != std::string::npos)
    {
        return path.substr(lastSep + 1);
    }

    return path;
}

std::string fnameToVarname(std::string fname)
{
    std::replace(fname.begin(), fname.end(), '.', '_');
    return fname;
}

std::string readFileToString(std::string path)
{
    std::ifstream file(path);
    if (file.fail()) {
        throw std::runtime_error("Could not open file " + path);
    }

    std::stringstream buf;
    buf << file.rdbuf();

    return buf.str();
}

void writeStringToFile(std::string path, std::string content)
{
    std::ofstream file(path, std::ios_base::out);
    if (file.fail())
    {
        throw std::runtime_error("Could not open file " + path);
    }

    file << content;
}

std::string charToHexLiteral(unsigned char c)
{
    std::stringstream buf;
    buf
        << "'\\x"
        << hexAlphabet[c>>4] // Most significant 4 bits. Unsigned char guarantees leading 0s after shift.
        << hexAlphabet[c&'\x0f'] // Least significant 4 bits
        << "'";
    return buf.str();
}

std::string buildHeaderString(std::string ns, std::string varname, std::string content)
{
    std::stringstream buf;
    buf
        << "#pragma once" << std::endl
        << std::endl
        << "namespace " << ns << std::endl
        << "{" << std::endl
        << indentation << "extern const char " << varname << "[" << content.size() << "];" << std::endl
        << "}" << std::endl;

    return buf.str();
}

std::string buildSourceString(std::string headerName, std::string ns, std::string varname, std::string content)
{
    std::stringstream buf;
    buf
        << "#include \"" << headerName << "\"" << std::endl
        << std::endl
        << "namespace " << ns << std::endl
        << "{" << std::endl
        << indentation << "const char " << varname << "[" << content.size() << "] = {" << std::endl;

    for (int i = 0; i < content.length(); i++)
    {
        // Limit number of char literals per line, maintain two tabs, and
        // add newlines after each line
        if (i % bytesPerLine == 0)
        {
            if (i != 0) buf << std::endl;
            buf << indentation << indentation;
        }

        buf << charToHexLiteral(content[i]);

        // Add a comma after each char literal except for the last. Add a
        // newline after all chars.
        if (i != content.length() - 1) buf << ",";
        else buf << std::endl;
    }

    buf
        << indentation << "};" << std::endl
        << "}" << std::endl;

    return buf.str();
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        usage("Wrong number of arguments.");
        return -1;
    }

    std::string ns = argv[1];
    if (!validateNamespace(ns))
    {
        usage("Invalid namespace identifier.");
        return -1;
    }

    std::string path = argv[2], content;
    try
    {
        content = readFileToString(path);
    }
    catch(const std::exception &e)
    {
        usage(e.what());
        return -1;
    }

    std::string
        fname = fnameFromPath(path),
        hPath = path + ".hpp",
        cPath = path + ".cpp",
        varname = fnameToVarname(fname),
        hContent = buildHeaderString(ns, varname, content),
        cContent = buildSourceString(fname + ".hpp", ns, varname, content);

    try
    {
        writeStringToFile(hPath, hContent);
        writeStringToFile(cPath, cContent);
    }
    catch(const std::exception &e)
    {
        usage(e.what());
        return -1;
    }

    return 0;
}
