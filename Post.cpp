//
// Post.cpp
//

#include "Post.h"

Post::Post
(Person * arg_author, Message * arg_msg)
{
	this->author = arg_author;
	this->msg = arg_msg;
	this->author = NULL;

	this->receivers = NULL;
	this->msg = NULL;
	this->links = NULL;
	this->actions = NULL;
	this->reactions = NULL;
	this->comments = NULL;
	this->keys = NULL;
	//this->history = NULL;
	this->created = NULL;
	this->updated = NULL;
	this->is_published = false;

}

Json::Value * Post::dumpJ
(void)
{
  int i;

  Json::Value * result_ptr = new Json::Value();
  Json::Value * dumpjv_ptr;
  //std::cout << ((int) (*(this->comments)).size()) << std::endl;
  // "actions"
  if (this->actions != NULL)
    {
	Json::Value json_actions;
	Json::Value json_actions_array;
	for (i = 0; i < (this->actions)->size(); i++) 
	{	
		dumpjv_ptr = ((*(this->actions))[i]->dumpJ());
		json_actions[i] = (*dumpjv_ptr);
		delete dumpjv_ptr;

	}
	json_actions_array["data"] = json_actions;
	json_actions_array["count"] = ((int) (*(this->actions)).size());
	(*result_ptr)["actions"] = json_actions_array;;
   
	}

  // comments
  if (this->comments != NULL)
    { 
	Json::Value json_comments;
	Json::Value json_comments_array;
	for (i = 0; i < (this->comments)->size(); i++)
	{
		dumpjv_ptr = ((*(this->comments))[i])->dumpJ();
		json_comments[i] = (*dumpjv_ptr);
		delete dumpjv_ptr;
	}
	json_comments_array["data"] = json_comments;
	//std::cout << json_comments << std::endl;
	json_comments_array["count"] = ((int) (*(this->comments)).size());
	//std::cout << json_comments_array["count"] << std::endl;
	(*result_ptr)["comments"] = json_comments_array;
    }

  // "links"
  if (this->links != NULL)
    {
	Json::Value json_links;
	Json::Value json_links_array;
	for (i = 0; i < (this->links)->size(); i++)
	{
		dumpjv_ptr = ((*(this->links))[i])->dumpJ();
		json_links[i] = (*dumpjv_ptr);
		delete dumpjv_ptr;
	}
	json_links_array["data"] = json_links;
	json_links_array["count"] = ((int) (*(this->links)).size());
	(*result_ptr)["links"] = json_links_array;
    }

  // "message"
  if (this->msg != NULL)
    {
	(*result_ptr)["message"] = (this->msg)->content;
	
    }

  // "reactions"
  if (this->reactions != NULL)
    {
      Json::Value json_reactions;
      Json::Value json_reactions_array;
      for (i = 0; i < (this->reactions)->size(); i++)
        {
          dumpjv_ptr = ((*(this->reactions))[i])->dumpJ();
          json_reactions[i] = (*dumpjv_ptr);
          delete dumpjv_ptr;
        }
      json_reactions_array["data"] = json_reactions;
      json_reactions_array["count"] = ((int) (*(this->reactions)).size());
      (*result_ptr)["reactions"] = json_reactions_array;
    }

  // "id"
  (*result_ptr)["id"] = (this->profile_id + "_" + this->post_id);

  // "from"
  if (this->author != NULL)
    {
      dumpjv_ptr = (this->author)->dumpJ();
      (*result_ptr)["from"] = (*dumpjv_ptr);
      delete dumpjv_ptr;
    }

  // "to"
  if (this->receivers != NULL)
    {
	
	Json::Value json_receivers;
	Json::Value json_receivers_array;

	dumpjv_ptr = (this->author)->dumpJ();
	(*result_ptr)["to"]["data"] = (*dumpjv_ptr);
	delete dumpjv_ptr;

	for (i = 0; i < (this->receivers)->size(); i++) {
		dumpjv_ptr = ((*(this->receivers))[i])->dumpJ();
		json_receivers[i] = (*dumpjv_ptr);
		delete dumpjv_ptr;
	}
	json_receivers_array["data"] = json_receivers;
	json_receivers_array["count"] = ((int) (*(this->receivers)).size());
	(*result_ptr)["to"] = json_receivers_array;
    }

  // "created_time"
  if (this->created != NULL)
    {
      (*result_ptr)["created_time"] = *((this->created)->getTimeString());
    }

  // "updated_time"
  if (this->updated != NULL)
    {
      (*result_ptr)["updated_time"] = *((this->updated)->getTimeString());
    }

  // "is_published"
  (*result_ptr)["is_published"] = this->is_published;

#ifdef _ECS36B_DEBUG_
  std::cout << result_ptr->toStyledString() << std::endl;
#endif /* _ECS36B_DEBUG_ */
  return result_ptr;
}

