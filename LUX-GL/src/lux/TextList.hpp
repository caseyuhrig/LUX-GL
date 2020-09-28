#pragma once

#include <string>
#include <map>
#include <memory>

#include "glfont/GLFont.h"
#include "glfont/FTLabel.h"


// MIT license for the font code https://github.com/jtberglund/GLFont/blob/master/LICENSE
// gonna re-write the code. reminder to ack. the orig. author.
// TODO Whenever the window is resized, you should update the window size of your label
// label->setWindowSize(windowWidth, windowSize);
namespace lux {

    class TextList
    {
    private:
        int _x, _y;
        int m_Width, m_Height;
        int _y_pos;
        std::map<unsigned int, std::shared_ptr<GLFont>> _fonts;
        // TODO make fonts static, etc...
        // res/fonts/Inconsolata/static/InconsolataCondensed-Medium.ttf
        // res/fonts/Inconsolata/static/InconsolataCondensed-Light.ttf
        //std::vector<std::unique_ptr<FTLabel>> _labels;
        std::vector<std::shared_ptr<FTLabel>> _labels;
    public:
        TextList() {}

        TextList(int x, int y, int width, int height) 
            : _x(x), _y(y), m_Width(width), m_Height(height), _y_pos(y)
        {
        }

        void Resize(const uint32_t& width, const uint32_t& height)
        {
            m_Width = width;
            m_Height = height;

            for (auto label : _labels)
            {
                label->setWindowSize(width, height);
                //label->setPosition(label->getX(), label->getY());
                //label->setText(label->getText());
            }
        }

        void AddText(unsigned int fontID, unsigned int labelID, unsigned int fontSize, int yOffset, const std::string& text)
        {
            auto font = _fonts.at(fontID);
            auto label = std::shared_ptr<FTLabel>(new FTLabel(font, text, _x, _y_pos, m_Width, m_Height));
            label->setColor(1.0, 1.0, 1.0, 1.0);
            label->setPixelSize(fontSize);
            //label->setSize(_width, _height);
            _labels.push_back(label);
            _y_pos += yOffset;
        }

        void AddFont(unsigned int fontID, const std::string& fontPath)
        {
            auto font = std::shared_ptr<GLFont>(new GLFont(fontPath));
            // TODO check if the font exists first
            _fonts.emplace(fontID, font);
        }

        void SetText(unsigned int labelID, const std::string& text) const
        {
            auto label = _labels.at(labelID);
            label->setText(text);
        }

        void Draw() const
        {
            for (auto label : _labels)
                label->render();
        }
    };
}