class PvPZone
{
    string Name;
    vector Center;
    float Radius;

    void PvPZone(string name, vector center, float radius)
    {
        Name = name;
        Center = center;
        Radius = radius;
    }

    bool Contains(vector position)
    {
        return vector.Distance(position, Center) <= Radius;
    }
}
