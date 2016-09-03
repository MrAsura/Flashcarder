#include "cardlist.h"
#include "global.h"
using global::c_type_id_t;

#include <list>
#include <algorithm>
#include <iterator>

Cardlist::Cardlist(): size_(0), first_(nullptr), last_(nullptr), current_(nullptr)
{

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

Cardlist *Cardlist::suffle()
{
    if( first_ == nullptr )
    {
        return this;
    }

    //Collect containers in a vector
    Container* cur = first_;
    std::vector<Container*> conts(0);

    while( cur->next != nullptr )
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
    return current_->card;
}

Cardlist::card_ptr Cardlist::next()
{
    current_ = current_->next;
    return current_->card;
}

Cardlist::card_ptr Cardlist::prev()
{
    current_ = current_->prev;
    return current_->card;
}

Cardlist::card_ptr Cardlist::first()
{
    return first_->card;
}

Cardlist::card_ptr Cardlist::last()
{
    return last_->card;
}

Cardlist *Cardlist::combine(Cardlist &list)
{
    size_ += list.size_;
    link(last_,list.first_);
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
    std::list<int> firsts(size);
    std::generate_n( firsts.begin(), size, []()->int{ static int num = 0; return num++; });
    std::list<int> seconds(firsts);

    std::vector<std::pair<int,int>> pairs(0);

    //pick random ind from first and second and remove them from the list
    while( firsts.size() > 1 )
    {
        std::uniform_int_distribution<int> d(0,firsts.size()-1);
        std::list<int>::iterator f_it = firsts.begin();
        std::list<int>::iterator s_it = seconds.begin();
        std::advance(f_it,d(e_));
        std::advance(s_it,d(e_));
        pairs.emplace_back(*f_it,*s_it);
        firsts.erase(f_it);
        seconds.erase(s_it);
    }

    fl = std::make_pair(*firsts.begin(),*seconds.begin());

    return pairs;
}
