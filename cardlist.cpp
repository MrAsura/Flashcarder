#include "cardlist.h"
#include "global.h"
using global::c_type_id_t;

#include <chrono>
#include <list>
#include <algorithm>
#include <iterator>

Cardlist::Cardlist(): size_(0), first_(nullptr), last_(nullptr), current_(nullptr)
{
    e_.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

Cardlist::Cardlist(Cardlist::card_ptr card)
{
    Container* cont = new Container(card);
    first_ = cont;
    last_ = cont;
    current_ = cont;
    size_ = 1;
}

Cardlist::~Cardlist()
{
    if( first_ == nullptr )
    {
        return;
    }

    Container* cur = first_;

    while( cur->next != nullptr )
    {
      cur = cur->next;
      delete cur->prev;
    }

    delete cur;
    first_ = nullptr;
    last_ = nullptr;
    current_ = nullptr;
    size_ = 0;
}

Cardlist *Cardlist::reset()
{
    current_ = first_;
    return this;
}

Cardlist *Cardlist::shuffle()
{
    if( first_ == nullptr )
    {
        return this;
    }

    //Collect containers in a vector
    Container* cur = first_;
    std::vector<Container*> conts(0);

    while( cur != nullptr )
    {
        conts.push_back(cur);
        cur = cur->next;
    }

    //will contain first and last index
    std::pair<int,int> fl(0,0);

    std::vector<std::pair<int,int>> order = getOrder(size_, fl);

    for( std::pair<int,int> ord: order )
    {
        link( conts.at(ord.first), conts.at(ord.second) );
    }

    first_ = conts.at(fl.first); //Find container index never used as the end point.
    first_->prev = nullptr;
    current_ = first_;
    last_ = conts.at(fl.second); //Find container index never used as the origin.
    last_->next = nullptr;

    return this;
}

Cardlist::card_ptr Cardlist::current()
{
    if (current_) return current_->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::next()
{
    if(current_ && current_->next) current_ = current_->next;
    if(current_) return current_->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::peek_next()
{
    if(current_ && current_->next) return current_->next->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::prev()
{
    if(current_ && current_->prev) current_ = current_->prev;
    if(current_) return current_->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::peek_prev()
{
    if(current_ && current_->prev) return current_->prev->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::first()
{
    if (first_) return first_->card;
    return nullptr;
}

Cardlist::card_ptr Cardlist::last()
{
    if (last_) return last_->card;
    return nullptr;
}

Cardlist *Cardlist::combine(Cardlist &list)
{
    if(list.size() == 0) return this;

    if(size_ != 0)
    {
        link(last_,list.first_);
    }
    else
    {
        first_ = list.first_;
        current_ = list.current_;
    }
    size_ += list.size_;
    last_ = list.last_;

    list.last_ = nullptr;
    list.first_ = nullptr;
    list.current_ = nullptr;
    list.size_ = 0;

    return this;
}

Cardlist *Cardlist::addCard(Cardlist::card_ptr card)
{
    Container* cont = new Container(card);

    if( first_ == nullptr )
    {
        first_ = cont;
        last_ = cont;
        current_ = cont;
    }
    else
    {
        link(last_,cont);
        last_ = cont;
    }

    ++size_;

    return this;
}

std::vector<Cardlist::card_ptr> Cardlist::findKeyword(QStringList keywords)
{
    std::vector<Cardlist::card_ptr> cards(0);
    Container* cur = first_;

    while( cur->next != nullptr )
    {
        for( QString kw: keywords)
        {
            QStringList klist = cur->card->keywords();
            if( klist.contains(kw) )
            {
                cards.push_back( cur->card );
            }
            cur = cur->next;
        }
    }

    return cards;
}

unsigned int Cardlist::size()
{
    return size_;
}

void Cardlist::link(Cardlist::Container *first, Cardlist::Container *second)
{
    first->next = second;
    second->prev = first;
}

std::vector<std::pair<int, int> > Cardlist::getOrder(unsigned int size, std::pair<int,int>& fl)
{
    std::vector<int> inds(size);
    unsigned tmp = 0;
    std::generate_n( inds.begin(), size, [&tmp]()->int{return tmp++; });
    std::shuffle(inds.begin(), inds.end(), e_);
    std::vector<std::pair<int,int>> pairs(0);

    for(unsigned i = 1; i < inds.size(); i++)
    {
        pairs.emplace_back(std::make_pair(inds[i-1], inds[i]));
    }

    fl = std::make_pair(inds.front(), inds.back());

    return pairs;
}
