#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>

typedef float __Wt;
static const __Wt kMAX_WEIGHT = 999999.f;

/// Adjacent type
template <class __V/*, typename __Wt*/>
using __Adj = std::pair<__V, __Wt>;

template<class __V>
bool operator==(__Adj<__V> const &p1, __Adj<__V> const &p2)
{
    return p1.first == p2.first;
}

template<class __V>
bool operator < (__Adj<__V> const &p1, __Adj<__V> const &p2)
{
    return p1.second < p2.second;
}



template <class __V>
using __Edge = std::pair<__V, __V>;

/// Directed edge hasher
template <class __V>
struct std::hash<__Edge<__V>>
{
    inline size_t operator()(__Edge<__V> const &v) const
    {
        return (std::hash<__V>()(v.first) << 1) ^ std::hash<__V>()(v.second);
    }
};

/// Directed edge equal
template <class __V>
struct std::equal_to<__Edge<__V>>
{
    inline bool operator()(__Edge<__V> const &e1, __Edge<__V> const &e2) const
    {
        return (e1.first==e2.first && e1.second==e2.second);
    }
};

/// Directed graph
template <class __V, class __EH=std::hash<__Edge<__V>>, class __EE=std::equal_to<__Edge<__V>>>
class Graph
{
public:

// struct Face
// {
//     std::vector<__V> points_;
// };

protected:
    std::unordered_map<__V, std::vector<__Adj<__V>>> vertices_;
    std::unordered_map<__Edge<__V>, __Wt, __EH, __EE> edges_;
    // std::unordered_map<__V, std::list<__Adj<__V>>> faces_;

public:
    inline size_t vertexSize() const {return vertices_.size();}
    inline size_t edgeSize() const {return edges_.size();}
    inline bool hasVertex(__V const &p) const {return vertices_.find(p) != vertices_.end();}
    inline bool hasEdge(__Edge<__V> const &p) const {return edges_.find(p) != edges_.end();}

    void addVertex(__V const &p);
    void addEdge(__V const &p1, __V const &p2, float weight);

    void remVertex(__V const &p);
    void remEdge(__V const &p1, __V const &p2);

    std::vector<__Adj<__V>> const &neighbors(__V const &p) {return vertices_[p];}
    // float cost(__V const &p1, __V const &p2) {return std::find(vertices_[p1].begin(),vertices_[p1].end(),p2).second;}

    template <class ___V, class ___EH, class ___EE>
    friend std::ostream &operator<<(std::ostream &os, Graph<___V, ___EH, ___EE> const &g);
};

template <class __V, class __EH, class __EE>
void Graph<__V, __EH, __EE>::addVertex(__V const &p)
{
    vertices_.insert({p,{}});
}

template <class __V, class __EH, class __EE>
void Graph<__V, __EH, __EE>::addEdge(__V const &p1, __V const &p2, float weight)
{
    /// add 2 enpoints
    addVertex(p1);
    addVertex(p2);
    /// add new edge
    edges_[{p1, p2}] = weight;
    /// add adjacents
    auto &end1_adj = vertices_[p1];
    auto ait = std::find_if(end1_adj.begin(), end1_adj.end(), [p2](__Adj<__V> const &adj){return adj.first == p2;});
    if(ait == end1_adj.end())
        end1_adj.push_back({p2, weight});
    else
        ait->second = weight;

    auto &end2_adj = vertices_[p2];
    ait = std::find_if(end2_adj.begin(), end2_adj.end(), [p1](__Adj<__V> const &adj){return adj.first == p1;});
    if(ait == end2_adj.end())
        vertices_[p2].push_back({p1, kMAX_WEIGHT});
}

template <class __V, class __EH, class __EE>
void Graph<__V, __EH, __EE>::remVertex(__V const &p)
{
    auto vit = vertices_.find(p);

    if(vit != vertices_.end())
    {
        /// remove all relation vertices & edges
        for(auto &[adj_vertex, list] : vertices_[p])
        {
            (void)(list);
            // auto &adj_vertex = adj.first;
            auto &adj_list = vertices_[adj_vertex];
            /// remove adjacents
            adj_list.erase(std::find_if(adj_list.begin(), adj_list.end(), [p](__Adj<__V> const &adj){return adj.first == p;}));
            /// remove protrudes
            auto eit = edges_.find({adj_vertex, p});
            if(eit != edges_.end()) edges_.erase(eit);

            eit = edges_.find({p, adj_vertex});
            if(eit != edges_.end()) edges_.erase(eit);
        }

        vertices_.erase(vit);
    }
}

template <class __V, class __EH, class __EE>
void Graph<__V, __EH, __EE>::remEdge(__V const &p1, __V const &p2)
{
    auto eit = edges_.find({p1, p2});

    if(eit != edges_.end())
    {
        /// remove/update all adjacents
        auto &adjs1 = vertices_[p1];
        auto &adjs2 = vertices_[p2];
        /// get adjacents
        auto a12 = std::find_if(adjs1.begin(), adjs1.end(), [p2](__Adj<__V> const &adj){return adj.first == p2;});
        auto a21 = std::find_if(adjs2.begin(), adjs2.end(), [p1](__Adj<__V> const &adj){return adj.first == p1;});
        assert(a12 != adjs1.end());
        assert(a21 != adjs2.end());
        /// no connected edge
        if(a21->second == kMAX_WEIGHT)
        {
            adjs1.erase(a12);
            adjs2.erase(a21);
        }
        else
        {
            a12->second = kMAX_WEIGHT;
        }
        /// remove edge
        edges_.erase(eit);
    }
}

template <class __V, class __EH, class __EE>
std::ostream &operator<<(std::ostream &os, Graph<__V, __EH, __EE> const &g)
{
    os <<"V: " << g.vertices_.size();
    for(auto &[vertex, adj_list] : g.vertices_)
    {
        os << "\n(" << vertex << ")";
        for(auto &[v, w] : adj_list)
        {
            // if(adj.second < kMAX_WEIGHT)
            os << "-(" << v << "|";
            (w < kMAX_WEIGHT) ? os << w : os << "INF" ;
            os << ")";
        }
    }

    os <<"\nE: " << g.edges_.size();
    for(auto &[end_point, weight] : g.edges_)
    {
        os << "\n(" << end_point.first << "-" << end_point.second << "|" << weight << ")";
    }

    return os;
}
