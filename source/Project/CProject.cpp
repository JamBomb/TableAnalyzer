//---------------------------------------------------------------------------


#pragma hdrstop

#include "CProject.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool CProject::Load(const char* FileName)
{
    if(NULL == FileName ||
       !XmlPraser.Load(FileName))
    {
        return false;
    }

    m_FileName = string(FileName);

    UpdateProjectNode();

    return true;
}



bool CProject::UpdateProjectNode()
{
    if(m_FileName == "")
    {
        return false;
    }

    XmlPraser.Load(m_FileName.c_str());

    NodeList.clear();

    TPrjNode Node;

    while(XmlPraser.FindElem("Project"))
    {
        Node.Name = XmlPraser.GetAttrib("name");
        Node.Value = XmlPraser.GetData();
        Node.Level = 0;
        NodeList.push_back(Node);
        m_ProjectCount++;

        XmlPraser.IntoElem();
        while(XmlPraser.FindElem("Module"))
        {
            Node.Name = XmlPraser.GetAttrib("name");
            Node.Value = XmlPraser.GetData();
            Node.Level = 1;
            NodeList.push_back(Node);

            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Item"))
            {
                Node.Name = XmlPraser.GetAttrib("name");
                Node.Value = XmlPraser.GetData();
                Node.Level = 2;
                NodeList.push_back(Node);
            }
            XmlPraser.OutOfElem();
        }
        XmlPraser.OutOfElem();
    }

    return true;
}

bool CProject::Add(const char* ParentName, const char* Name, const char* Value)
{
    if(m_FileName == "")
    {
        return false;
    }

    XmlPraser.ResetPos();

    if(NULL == ParentName) // if it is null, it is top node
    {
        if(XmlPraser.FindElem("Project"))
        {
            XmlPraser.SetAttrib("name", Name);
        }
        else
        {
            XmlPraser.AddElem("Project", Value);
            XmlPraser.SetAttrib("name", Name);
        }
    }
    else
    {
        string Attr ="";
        while(XmlPraser.FindElem("Project"))
        {
            Attr = XmlPraser.GetAttrib("name");
            if(Attr == string(ParentName))
            {
                XmlPraser.AddChildElem("Module", Value);
                XmlPraser.AddChildAttrib("name", Name);
                break;
            }
            else
            {
                XmlPraser.IntoElem();
                while(XmlPraser.FindElem("Module"))
                {
                    Attr = XmlPraser.GetAttrib("name");
                    if(Attr == string(ParentName))
                    {
                        XmlPraser.AddChildElem("Item", Value);
                        XmlPraser.AddChildAttrib("name", Name);
                        break;
                    }
                }
                XmlPraser.OutOfElem();
            }
        }
    }

    XmlPraser.Save(m_FileName.c_str());

    UpdateProjectNode();

    return true;

}


bool CProject::Delete(const char* ParentName, const char* Name)
{
    if(m_FileName == "" || NULL == ParentName)
    {
        return false;
    }

    string Attr ="";
    XmlPraser.ResetPos();
    while(XmlPraser.FindElem("Project"))
    {
        Attr = XmlPraser.GetAttrib("name");
        if(Attr == string(ParentName))
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(Name))
                {
                    // while(XmlPraser.FindChildElem("Item"))
                    // {
                    //       XmlPraser.RemoveChildElem();
                    // }

                    XmlPraser.RemoveElem();
                    break;
                }

            }
            XmlPraser.OutOfElem();
        }
        else
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(ParentName))
                {
                    while(XmlPraser.FindChildElem("Item"))
                    {
                        Attr = XmlPraser.GetChildAttrib("name");
                        if(Attr == string(Name))
                        {
                            XmlPraser.RemoveChildElem();
                            break;
                        }
                    }
                }
            }
            XmlPraser.OutOfElem();
        }
    }

    XmlPraser.Save(m_FileName.c_str());

    UpdateProjectNode();

    return true;
}

bool CProject::IsExisted(const char* ParentName, const char* Name)
{
    if(m_FileName == "" || NULL == ParentName
       || NULL == Name)
    {
        return true;
    }

    string Attr ="";
    XmlPraser.ResetPos();
    while(XmlPraser.FindElem("Project"))
    {
        Attr = XmlPraser.GetAttrib("name");
        if(Attr == string(ParentName))
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(Name))
                {

                    return true;
                }

            }
            XmlPraser.OutOfElem();
        }
        else
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(ParentName))
                {
                    while(XmlPraser.FindChildElem("Item"))
                    {
                        Attr = XmlPraser.GetChildAttrib("name");
                        if(Attr == string(Name))
                        {
                            return true;

                        }
                    }
                }
            }
            XmlPraser.OutOfElem();
        }
    }


    return false;
}

bool CProject::SetValue(const char* ParentName, const char* Name, const char* Value)
{
    if(m_FileName == "" || NULL == Name)
    {
        return false;
    }

    string Attr ="";
    XmlPraser.ResetPos();
    if(ParentName == NULL)
    {
        if(XmlPraser.FindElem("Project"))
        {
            Attr = XmlPraser.GetAttrib("name");
            if(Attr == string(Name))
            {
                XmlPraser.SetData(Value);
                XmlPraser.Save(m_FileName.c_str());
                return true;
            }
        }

        return false;
    }

    while(XmlPraser.FindElem("Project"))
    {
        Attr = XmlPraser.GetAttrib("name");
        if(Attr == string(ParentName))
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(Name))
                {
                    XmlPraser.SetData(Value);
                    XmlPraser.Save(m_FileName.c_str());
                    return true;
                }

            }
            XmlPraser.OutOfElem();
        }
        else
        {
            XmlPraser.IntoElem();
            while(XmlPraser.FindElem("Module"))
            {
                Attr = XmlPraser.GetAttrib("name");
                if(Attr == string(ParentName))
                {
                    while(XmlPraser.FindChildElem("Item"))
                    {
                        Attr = XmlPraser.GetChildAttrib("name");
                        if(Attr == string(Name))
                        {
                            XmlPraser.SetChildData(Value);
                            XmlPraser.Save(m_FileName.c_str());

                            return true;
                        }
                    }
                }
            }
            XmlPraser.OutOfElem();
        }
    }

    return false;
}

string CProject::GetPrjName(DWORD PrjIdx)
{
    string rtn = "";
    DWORD prjcnt = 0;

    if(PrjIdx < m_ProjectCount)
    {
        for (DWORD i = 0; i < NodeList.size(); i++)
        {
            if(NodeList[i].Level == 0)
            {
                if(PrjIdx == prjcnt)
                {
                    rtn = NodeList[i].Name;
                    break;
                }

                prjcnt++;
            }
        }
    }

    return rtn;

}

